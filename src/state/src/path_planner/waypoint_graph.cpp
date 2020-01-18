/**
 * @file waypoint_graph.h
 * Graph which uses waypoints for path
 */

#include <utility>

#include "state/path_planner/waypoint_graph.h"

namespace state {

WaypointGraph::WaypointGraph(std::size_t p_map_size,
                             std::vector<std::vector<bool>> p_valid_terrain)
    : map_size(p_map_size), valid_terrain(std::move(p_valid_terrain)),
      graph(std::make_unique<Graph>()) {}

void WaypointGraph::setValidTerrain(
    std::vector<std::vector<bool>> p_valid_terrain) {
  valid_terrain = std::move(p_valid_terrain);

  recomputeWaypointGraph();
}

void WaypointGraph::addObstacle(const DoubleVec2D &position) {
  valid_terrain[position.x][position.y] = false;
}

void WaypointGraph::addObstacles(const std::vector<DoubleVec2D> &positions) {
  for (auto position : positions) {
    addObstacle(position);
  }
}

void WaypointGraph::removeObstacle(const DoubleVec2D &position) {
  valid_terrain[position.x][position.y] = true;
}

boost::unordered_set<DoubleVec2D> WaypointGraph::getWaypoints() const {
  return graph->getNodes();
}

bool WaypointGraph::isValidPosition(const double_t &x,
                                    const double_t &y) const {
  if (x < 0 || y < 0)
    return false;

  if (x >= map_size || y >= map_size)
    return false;

  return valid_terrain[std::floor(x)][std::floor(y)];
}

bool WaypointGraph::isValidPosition(const DoubleVec2D &position) const {
  return isValidPosition(position.x, position.y);
}

void WaypointGraph::resetWaypointGraph() { graph->resetGraph(); }

void WaypointGraph::addWaypoint(DoubleVec2D position) {
  graph->addNode(position);
  recomputeWaypointEdges(position);
}

void WaypointGraph::removeWaypoint(DoubleVec2D position) {
  graph->removeNode(position);
}

bool WaypointGraph::isStraightLineTraversable(DoubleVec2D start,
                                              DoubleVec2D destination) const {
  if (start > destination)
    std::swap(start, destination);

  if (start.x == destination.x) {
    // Both points are on a vertical line along

    // True, if x is an integral value
    bool isIntegralX = false;
    if (start.x == std::floor(start.x)) {
      isIntegralX = true;
    }

    for (auto y = std::floor(start.y); y < std::floor(destination.y); y++) {
      if (destination.y == y)
        break;

      if (isIntegralX) {
        if (!isValidPosition(start.x, y) && !isValidPosition(start.x - 1, y))
          return false;
      } else {
        if (!isValidPosition(start.x, start.y))
          return false;
      }
    }
  } else if (start.y == destination.y) {
    bool isIntegralY = false;
    if (start.y == std::floor(start.y)) {
      isIntegralY = true;
    }

    for (auto x = std::floor(start.x); x < std::floor(destination.x); x++) {
      if (destination.x == x)
        break;

      if (isIntegralY) {
        if (!isValidPosition(x, start.y) && !isValidPosition(x, start.y - 1))
          return false;
      } else {
        if (!isValidPosition(start.x, start.y))
          return false;
      }
    }
  } else {
    throw std::domain_error("The points must be on the same line");
  }

  return true;
}

bool WaypointGraph::arePointsDirectlyReachable(DoubleVec2D point_a,
                                               DoubleVec2D point_b) const {
  // They are the same point
  if (point_a == point_b)
    return true;

  if (point_a > point_b)
    std::swap(point_a, point_b);

  if (point_a.x != point_b.x) {

    if (point_a.y == point_b.y)
      return isStraightLineTraversable(point_a, point_b);

    // Get all integral x's which the line joining a and b intersect
    auto x_intersections = generateIntersections(point_a.x, point_b.x);
    auto slope = getSlope(point_a, point_b);

    for (std::size_t i = 0; i < (x_intersections.size() - 1); i++) {
      auto current_x = x_intersections[i];
      auto next_x = x_intersections[i + 1];

      // y = slope*(x - x1) + y1
      auto current_y = slope * (current_x - point_a.x) + point_a.y;
      auto next_y = slope * (next_x - point_a.x) + point_a.y;

      if (current_y > next_y)
        std::swap(current_y, next_y);

      for (auto y = std::floor(current_y); y <= std::floor(next_y); y++) {
        // Check if position being traversed is valid
        // If y is next_y, ending at next_y so needn't check that position
        if (!isValidPosition(current_x, y) && y != next_y) {
          return false;
        }
      }
    }
  } else
    return isStraightLineTraversable(point_a, point_b);

  return true;
}

void WaypointGraph::recomputeWaypoints() {
  auto corner_offsets =
      std::vector<DoubleVec2D>{{0, 0}, {0, 1}, {1, 0}, {1, 1}};

  // All 4 points at the corners of an obstacle are waypoints
  for (std::size_t row = 0; row < map_size; row++) {
    for (std::size_t col = 0; col < map_size; col++) {
      if (valid_terrain[row][col])
        continue;

      for (auto corner_offset : corner_offsets) {
        auto corner_x = row + corner_offset.x;
        auto corner_y = col + corner_offset.y;

        // Ignore points on the map borders
        if (corner_x >= map_size || corner_y >= map_size || corner_x == 0 ||
            corner_y == 0)
          continue;

        graph->addNode({corner_x, corner_y});
      }
    }
  }

  // If there exists a waypoint which is lying between two adjacent waypoints,
  // it is redundant
  for (auto waypoint : graph->getNodes()) {
    auto x = waypoint.x;
    auto y = waypoint.y;

    if (!(isValidPosition(x, y) || isValidPosition(x - 1, y)) ||
        !(isValidPosition(x, y) || isValidPosition(x, y - 1)) ||
        !(isValidPosition(x - 1, y - 1) || isValidPosition(x, y - 1)) ||
        !(isValidPosition(x - 1, y - 1) || isValidPosition(x - 1, y))) {
      graph->removeNode(waypoint);
    }
  }
}

void WaypointGraph::recomputeWaypointEdges(DoubleVec2D position) {
  if (!graph->checkNodeExists(position))
    return;

  for (auto waypoint : graph->getNodes()) {
    if (waypoint == position)
      continue;

    if (arePointsDirectlyReachable(waypoint, position)) {
      graph->addEdge(waypoint, position, position.distance(waypoint));
    }
  }
}

void WaypointGraph::recomputeWaypointEdges() {
  for (auto waypoint : graph->getNodes()) {
    recomputeWaypointEdges(waypoint);
  }
}

void WaypointGraph::recomputeWaypointGraph() {
  // Remove any previous waypoints and edges
  resetWaypointGraph();

  recomputeWaypoints();
  recomputeWaypointEdges();
}

std::vector<DoubleVec2D> WaypointGraph::getPath(DoubleVec2D start_position,
                                                DoubleVec2D end_position) {
  addWaypoint(start_position);
  addWaypoint(end_position);

  auto result = graph->getPath(start_position, end_position);

  removeWaypoint(start_position);
  removeWaypoint(end_position);

  return result;
}

} // namespace state
