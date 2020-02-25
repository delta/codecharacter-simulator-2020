/**
 * @file path_planner.cpp
 */

#include "state/path_planner/path_planner.h"

#include <utility>

namespace state {

PathPlanner::PathPlanner(std::unique_ptr<Map> p_map) : map(std::move(p_map)) {
    auto map_size = map->getSize();
    auto valid_terrain = std::vector<std::vector<bool>>(
        map_size, std::vector<bool>(map_size, true));

    for (size_t row = 0; row < map_size; row++) {
        for (size_t col = 0; col < map_size; col++) {
            auto position_terrain =
                map->getTerrainType((size_t) row, (size_t) col);

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
                               ActorType actor_type) {
    if (position.x < 0 || position.y < 0 || position.x > map->getSize() ||
        position.y > map->getSize()) {
        return false;
    }

    // For bot, any position with 0 <= x <= MAP_SIZE and 0 <= y <= MAP_SIZE is
    // valid position
    if (actor_type == ActorType::BOT)
        return true;

    switch (player_id) {

    // For player 1, the top (x, MAP_SIZE) and right (MAP_SIZE, y) map borders
    // are invalid tower positions
    case PlayerId::PLAYER1:
        return !(position.x == map->getSize() || position.y == map->getSize());

    // For player 2, the bottom (x, 0) and left (0, y) map borders
    // are invalid tower positions
    case PlayerId::PLAYER2:
        return !(position.x == 0 || position.y == 0);

    default:
        return true;
    }
}

Vec2D PathPlanner::getOffset(const DoubleVec2D &position,
                             const PlayerId &player_id) {
    if (!isInMapRange(position, player_id, ActorType::TOWER)) {
        return Vec2D::null;
    }

    switch (player_id) {

    // For player 1, the offset tile is one containing position.
    // In case of integral (x,y) the tile having position as lower left corner
    case PlayerId::PLAYER1: {
        return {(int64_t) std::floor(position.x),
                (int64_t) std::floor(position.y)};
    }

    // For player 2, the offset tile is one containing position.
    // In case of integral (x,y) the tile having position as upper right corner
    // Return the lower left position of the offset
    case PlayerId::PLAYER2: {
        return {(int64_t) std::ceil(position.x) - 1,
                (int64_t) std::ceil(position.y) - 1};
    }

    default:
        return Vec2D ::null;
    }
}

bool PathPlanner::isValidTowerPosition(DoubleVec2D position,
                                       PlayerId player_id) {
    auto tower_base_position = getOffset(position, player_id);

    if (!tower_base_position) {
        return false;
    }

    return path_graph.isValidPosition(tower_base_position);
}

std::vector<Vec2D> PathPlanner::getAdjoiningOffsets(DoubleVec2D position) {
    // Eg: For (1.5, 1.5), result = {(1, 1)}
    // For (1, 1), result = {(0,0), (0,1), (1,0), (1, 1)}
    // For (0.5, 1), result = {(0,0), (0,1)}

    std::vector<double_t> xs;
    std::vector<double_t> ys;

    DoubleVec2D position_floor = {std::floor(position.x),
                                  std::floor(position.y)};

    xs.push_back(position_floor.x);
    // If x is integral, both offsets including x are taken
    if (position.x == position_floor.x) {
        xs.push_back(position_floor.x - 1);
    }

    ys.push_back(position_floor.y);
    // If y is integral, both offsets including y are taken
    if (position.y == position_floor.y) {
        ys.push_back(position_floor.y - 1);
    }

    std::vector<Vec2D> ans;

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
    auto adjacent_tiles = getAdjoiningOffsets(position);

    // If one of the adjacent tiles is traversable, the position can be a valid
    // position for a bot
    for (auto adjacent_tile : adjacent_tiles) {
        if (path_graph.isValidPosition(adjacent_tile)) {
            return true;
        }
    }

    return false;
}

Vec2D PathPlanner::buildTower(DoubleVec2D position, PlayerId player_id) {
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

bool PathPlanner::destroyTower(Vec2D tower_offset) {
    if (tower_offset == DoubleVec2D::null) {
        throw std::invalid_argument("Invalid tower build position for player");
    }

    auto position_terrain =
        map->getTerrainType((size_t) tower_offset.x, (size_t) tower_offset.y);
    if (position_terrain == TerrainType::WATER ||
        !isOffsetBlocked(tower_offset)) {
        return false;
    }

    path_graph.removeObstacle(tower_offset);
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

TerrainType PathPlanner::getTerrainType(DoubleVec2D position) const {
    if (!path_graph.isValidPosition(position)) {
        throw "Position is not within the map\n";
    }

    size_t pos_x = std::floor(position.x), pos_y = std::floor(position.y);
    size_t map_size = map->getSize();

    if (pos_x == map_size) {
        --pos_x;
    }
    if (pos_y == map_size) {
        --pos_y;
    }

    return map->getTerrainType(pos_x, pos_y);
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

        // Distance that can be travelled in a straight line with no turns
        double travel_distance =
            std::min(distance_left, current_position.distance(next_position));

        current_position =
            getPointAlongLine(current_position, next_position, travel_distance);
        distance_left -= travel_distance;
    }

    return current_position;
}

} // namespace state
