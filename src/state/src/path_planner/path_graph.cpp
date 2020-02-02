/**
 * @file path_graph.cpp
 * Declares a wrapper for a graph for path calculations
 */

#include <utility>

#include "state/path_planner/path_graph.h"

namespace state {

PathGraph::PathGraph() = default;

PathGraph::PathGraph(size_t p_map_size,
                     std::vector<std::vector<bool>> p_valid_terrain,
                     Graph p_graph)
    : map_size(p_map_size), valid_terrain(std::move(p_valid_terrain)),
      graph(std::move(p_graph)) {}

void PathGraph::setValidTerrain(
    std::vector<std::vector<bool>> p_valid_terrain) {
    valid_terrain = std::move(p_valid_terrain);

    recomputeWaypointGraph();
}

void PathGraph::addObstacle(const DoubleVec2D &position) {
    if (position.x < 0 || position.x >= map_size || position.y < 0 ||
        position.y >= map_size) {
        throw std::domain_error("Position not inside the range of map");
    }

    valid_terrain[std::floor(position.x)][std::floor(position.y)] = false;
}

void PathGraph::addObstacles(const std::vector<DoubleVec2D> &positions) {
    for (auto const &position : positions) {
        addObstacle(position);
    }
}

void PathGraph::removeObstacle(const DoubleVec2D &position) {
    if (position.x < 0 || position.x >= map_size || position.y < 0 ||
        position.y >= map_size) {
        throw std::domain_error("Position not inside the range of map");
    }

    valid_terrain[std::floor(position.x)][std::floor(position.y)] = true;
}

boost::unordered_set<DoubleVec2D> PathGraph::getWaypoints() const {
    return graph.getNodes();
}

bool PathGraph::isValidPosition(double_t x, double_t y) const {
    if (x < 0 || y < 0)
        return false;

    if (x > map_size || y > map_size)
        return false;

    if (x == map_size)
        x--;
    if (y == map_size)
        y--;

    return valid_terrain[std::floor(x)][std::floor(y)];
}

bool PathGraph::isValidPosition(const DoubleVec2D &position) const {
    return isValidPosition(position.x, position.y);
}

void PathGraph::resetWaypointGraph() { graph.resetGraph(); }

void PathGraph::addWaypoint(const DoubleVec2D &position) {
    graph.addNode(position);
    recomputeWaypointEdges(position);
}

void PathGraph::removeWaypoint(const DoubleVec2D &position) {
    graph.removeNode(position);
}

bool PathGraph::isStraightLineTraversable(DoubleVec2D start,
                                          DoubleVec2D destination) const {
    if (start > destination)
        std::swap(start, destination);

    if (start.x == destination.x) {
        // Both points are on a vertical line along the same X value

        // True, if x is an integral value
        bool isIntegralX = (start.x == std::floor(start.x));

        for (int64_t y = std::floor(start.y); y < std::floor(destination.y);
             y++) {
            if (isIntegralX) {
                if (!isValidPosition(start.x, y) &&
                    !isValidPosition(start.x - 1, y))
                    return false;
            } else if (!isValidPosition(start.x, y)) {
                return false;
            }
        }
    } else if (start.y == destination.y) {
        bool isIntegralY = (start.y == std::floor(start.y));

        for (int64_t x = std::floor(start.x); x < std::floor(destination.x);
             x++) {
            if (destination.x == (double_t)x)
                break;

            if (isIntegralY) {
                if (!isValidPosition(x, start.y) &&
                    !isValidPosition(x, start.y - 1))
                    return false;
            } else if (!isValidPosition(x, start.y)) {
                return false;
            }
        }
    } else {
        throw std::domain_error("The points must be on the same line");
    }

    return true;
}

bool PathGraph::arePointsDirectlyReachable(DoubleVec2D point_a,
                                           DoubleVec2D point_b) const {
    // They are the same point
    if (point_a == point_b)
        return true;

    if (point_a > point_b)
        std::swap(point_a, point_b);

    // Points are on vertical/horizontal line
    if (point_a.y == point_b.y || point_a.x == point_b.x) {
        return isStraightLineTraversable(point_a, point_b);
    }

    // Get all integral x's which the line joining a and b intersect
    auto x_intersections = generateIntersections(point_a.x, point_b.x);
    auto slope = getSlope(point_a, point_b);

    for (size_t i = 0; i < (x_intersections.size() - 1); i++) {
        auto current_x = x_intersections[i];
        auto next_x = x_intersections[i + 1];

        // y = slope*(x - x1) + y1
        auto current_y = slope * (current_x - point_a.x) + point_a.y;
        auto next_y = slope * (next_x - point_a.x) + point_a.y;

        if (current_y > next_y)
            std::swap(current_y, next_y);

        for (int64_t y = std::floor(current_y); y <= std::floor(next_y); y++) {
            // Check if position being traversed is valid
            // If y is next_y, ending at next_y so needn't check that
            // position
            if (!isValidPosition(current_x, y) && (double_t)y != next_y) {
                return false;
            }
        }
    }

    return true;
}

void PathGraph::recomputeWaypoints() {
    auto corner_offsets =
        std::vector<DoubleVec2D>{{0, 0}, {0, 1}, {1, 0}, {1, 1}};

    // All 4 points at the corners of an obstacle are waypoints
    for (size_t row = 0; row < map_size; row++) {
        for (size_t col = 0; col < map_size; col++) {
            if (valid_terrain[row][col])
                continue;

            for (auto corner_offset : corner_offsets) {
                auto corner_x = row + corner_offset.x;
                auto corner_y = col + corner_offset.y;

                // Ignore points on the map borders
                if (corner_x >= map_size || corner_y >= map_size ||
                    corner_x == 0 || corner_y == 0)
                    continue;

                graph.addNode({corner_x, corner_y});
            }
        }
    }

    // If there exists a waypoint which is lying between two adjacent waypoints,
    // it is redundant
    for (auto const &waypoint : graph.getNodes()) {
        auto x = waypoint.x;
        auto y = waypoint.y;

        bool removeNode = false;

        if (!(isValidPosition(x, y) || isValidPosition(x - 1, y))) {
            // Both tiles to the right are blocked
            removeNode = true;
        } else if (!(isValidPosition(x, y) || isValidPosition(x, y - 1))) {
            // Both tiles to the top are blocked
            removeNode = true;
        } else if (!(isValidPosition(x - 1, y - 1) ||
                     isValidPosition(x, y - 1))) {
            // Both tiles to the left are blocked
            removeNode = true;
        } else if (!(isValidPosition(x - 1, y - 1) ||
                     isValidPosition(x - 1, y))) {
            // Both tiles to the bottom are blocked
            removeNode = true;
        }

        if (removeNode) {
            graph.removeNode(waypoint);
        }
    }
}

void PathGraph::recomputeWaypointEdges(const DoubleVec2D &position) {
    if (!graph.checkNodeExists(position))
        return;

    for (auto const &waypoint : graph.getNodes()) {
        if (waypoint == position)
            continue;

        if (arePointsDirectlyReachable(waypoint, position)) {
            graph.addEdge(waypoint, position, position.distance(waypoint));
        }
    }
}

void PathGraph::recomputeWaypointEdges() {
    for (auto const &waypoint : graph.getNodes()) {
        recomputeWaypointEdges(waypoint);
    }
}

void PathGraph::recomputeWaypointGraph() {
    // Remove any previous waypoints and edges
    resetWaypointGraph();

    recomputeWaypoints();
    recomputeWaypointEdges();
}

std::vector<DoubleVec2D> PathGraph::getPath(DoubleVec2D start_position,
                                            DoubleVec2D end_position) {
    addWaypoint(start_position);
    addWaypoint(end_position);

    auto result = graph.getPath(start_position, end_position);

    removeWaypoint(start_position);
    removeWaypoint(end_position);

    return result;
}

} // namespace state
