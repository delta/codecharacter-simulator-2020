/**
 * @file path_planner.cpp
 */

#include "state/path_planner/path_planner.h"

#include <utility>

namespace state {

PathPlanner::PathPlanner(std::unique_ptr<Map> p_map) : map(std::move(p_map)) {
    auto map_size = map->GetSize();
    auto valid_terrain = std::vector<std::vector<bool>>(
        map_size, std::vector<bool>(map_size, true));

    for (size_t row = 0; row < map_size; row++) {
        for (size_t col = 0; col < map_size; col++) {
            auto position_terrain =
                map->GetTerrainType((size_t)row, (size_t)col);

            // The square from (row, col) to (row+1,col+1) is of this terrain
            valid_terrain[row][col] = (position_terrain == TerrainType::LAND ||
                                       position_terrain == TerrainType::FLAG);
        }
    }

    Graph graph = Graph();
    path_graph = PathGraph(map_size, valid_terrain, graph);
}

bool PathPlanner::isOffsetBlocked(const Vec2D &position) const {
    return !path_graph.isValidPosition(position);
}

bool PathPlanner::isInMapRange(DoubleVec2D position, state::PlayerId player_id,
                               bool is_tower) {
    if (position.x < 0 || position.y < 0 || position.x > map->GetSize() ||
        position.y > map->GetSize()) {
        return false;
    }

    // For bot, any position with 0 <= x <= MAP_SIZE and 0 <= y <= MAP_SIZE is
    // valid position
    if (!is_tower)
        return true;

    switch (player_id) {

    // For player 1, the top (x, MAP_SIZE) and right (MAP_SIZE, y) map borders
    // are invalid tower positions
    case PlayerId::PLAYER1:
        return !(position.x == map->GetSize() || position.y == map->GetSize());

    // For player 2, the bottom (x, 0) and left (0, y) map borders
    // are invalid tower positions
    case PlayerId::PLAYER2:
        return !(position.x == 0 || position.y == 0);

    default:
        return true;
    }
}

Vec2D PathPlanner::getOffset(DoubleVec2D position, state::PlayerId player_id) {
    if (!isInMapRange(position, player_id, true)) {
        return Vec2D::null;
    }

    switch (player_id) {

    // For player 1, the offset tile is one containing position.
    // In case of integral (x,y) the tile having position as lower left corner
    case PlayerId::PLAYER1: {
        position.x = std::floor(position.x);
        position.y = std::floor(position.y);
        return {(int64_t)position.x, (int64_t)position.y};
    }

    // For player 2, the offset tile is one containing position.
    // In case of integral (x,y) the tile having position as upper right corner
    // Return the lower left position of the offset
    case PlayerId::PLAYER2: {
        position.x = std::ceil(position.x);
        position.y = std::ceil(position.y);
        return {(int64_t)position.x - 1, (int64_t)position.y - 1};
    }

    default:
        return Vec2D ::null;
    }
}

bool PathPlanner::isValidTowerPosition(DoubleVec2D position,
                                       PlayerId player_id) {
    auto tower_base_position = getOffset(position, player_id);

    if (tower_base_position == Vec2D::null) {
        return false;
    }

    return path_graph.isValidPosition(tower_base_position);
}

std::vector<DoubleVec2D> PathPlanner::getAdjacentOffsets(DoubleVec2D position) {
    std::vector<double_t> xs;
    std::vector<double_t> ys;

    DoubleVec2D position_floor = {std::floor(position.x),
                                  std::floor(position.y)};

    xs.push_back(position_floor.x);
    if (position.x == position_floor.x) {
        xs.push_back(position_floor.x - 1);
    }

    ys.push_back(position_floor.y);
    if (position.y == position_floor.y) {
        ys.push_back(position_floor.y - 1);
    }

    std::vector<DoubleVec2D> ans;

    for (auto x : xs) {
        for (auto y : ys) {
            if (path_graph.isValidPosition({x, y})) {
                ans.emplace_back(x, y);
            }
        }
    }

    return ans;
}

bool PathPlanner::isValidBotPosition(DoubleVec2D position) {
    auto adjacent_tiles = getAdjacentOffsets(position);

    for (auto adjacent_tile : adjacent_tiles) {
        if (path_graph.isValidPosition(adjacent_tile)) {
            return true;
        }
    }

    return false;
}

DoubleVec2D PathPlanner::buildTower(DoubleVec2D position, PlayerId player_id) {
    DoubleVec2D tower_offset = getOffset(position, player_id);

    if (tower_offset == DoubleVec2D::null) {
        throw std::invalid_argument("Invalid tower build position for player");
    }

    if (isOffsetBlocked(tower_offset)) {
        return DoubleVec2D::null;
    }

    path_graph.addObstacle(tower_offset);
    return tower_offset;
}

bool PathPlanner::destroyTower(DoubleVec2D position) {
    if (position == DoubleVec2D::null) {
        throw std::invalid_argument("Invalid tower position for player");
    }

    auto position_terrain = map->GetTerrainType(position.x, position.y);
    if (position_terrain == TerrainType::WATER || !isOffsetBlocked(position)) {
        return false;
    }

    path_graph.removeObstacle(position);
    return true;
}

void PathPlanner::recomputePathGraph() { path_graph.recomputeWaypointGraph(); }

DoubleVec2D PathPlanner::getPointAlongLine(const DoubleVec2D &point_a,
                                           const DoubleVec2D &point_b,
                                           const double_t &distance) {
    if (point_a == point_b) {
        throw std::invalid_argument("Start and end points cannot be same");
    }

    double_t magnitude = point_a.distance(point_b);
    DoubleVec2D unit_vector = {(point_b.x - point_a.x) / magnitude,
                               (point_b.y - point_a.y) / magnitude};

    return (point_a + unit_vector * distance);
}

DoubleVec2D PathPlanner::getNextPosition(DoubleVec2D source,
                                         DoubleVec2D destination,
                                         size_t speed) {
    std::vector<DoubleVec2D> path = path_graph.getPath(source, destination);

    if (path.empty()) {
        return DoubleVec2D::null;
    }

    double distance_left = speed;
    DoubleVec2D current_position = source;

    for (auto next_position : path) {
        if (distance_left == 0 || current_position == destination)
            break;

        if (current_position.distance(next_position) >= distance_left) {
            current_position = getPointAlongLine(current_position,
                                                 next_position, distance_left);
            distance_left = 0;
        } else {
            distance_left -= current_position.distance(next_position);
            current_position = next_position;
        }
    }

    return current_position;
}

} // namespace state
