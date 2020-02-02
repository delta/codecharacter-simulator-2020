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
                map->GetTerrainType((int64_t)row, (int64_t)col);

            // The square from (row, col) to (row+1,col+1) is of this terrain
            valid_terrain[row][col] = (position_terrain == TerrainType::LAND ||
                                       position_terrain == TerrainType::FLAG);
        }
    }

    Graph graph = Graph();
    path_graph = PathGraph(map_size, valid_terrain, graph);
}

bool PathPlanner::isPositionBlocked(DoubleVec2D position) {
    return !path_graph.isValidPosition(position);
}

void PathPlanner::addTower(DoubleVec2D position) {
    if (isPositionBlocked(position)) {
        throw std::invalid_argument("Position is already blocked");
    }

    path_graph.addObstacle(position);
}

void PathPlanner::removeTower(DoubleVec2D position) {
    auto position_terrain = map->GetTerrainType(position.x, position.y);
    if (position_terrain == TerrainType::WATER ||
        !isPositionBlocked(position)) {
        throw std::invalid_argument("No tower in position");
    }

    path_graph.removeObstacle(position);
}

void PathPlanner::recomputePathGraph() { path_graph.recomputeWaypointGraph(); }

DoubleVec2D PathPlanner::getPointAlongLine(DoubleVec2D point_a,
                                           DoubleVec2D point_b,
                                           double_t distance) {
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
