/**
 * @file waypoint_graph_helper.cpp
 * Declares helper functions for waypoint graph
 */

#include "state/path_planner/path_graph.h"

namespace state {

std::vector<double_t> PathGraph::generateIntersections(double_t a,
                                                       double_t b) const {
  if (a > b)
    std::swap(a, b);

  auto result = std::vector<double_t>();

  for (auto x = std::ceil(a); x <= std::floor(b); x++) {
    result.push_back(x);
  }

  if (result.empty() || result[0] != a) {
    result.insert(result.begin(), a);
  }

  if (result.empty() || result.back() != b) {
    result.push_back(b);
  }

  return result;
}

double_t PathGraph::getSlope(const DoubleVec2D &a, const DoubleVec2D &b) const {
  return (b.y - a.y) / (b.x - a.x);
}

} // namespace state
