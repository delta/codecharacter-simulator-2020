/**
 * @file node_edge_list.h
 * Declares a single node and the edges to other nodes
 */

#pragma once

#include "physics/vector.hpp"
#include "state/path_planner/graph/node.h"
#include <boost/unordered_map.hpp>

namespace state {

struct NodeEdgeList {
  /**
   * Start Node
   */
  Node node;

  /**
   * Map of all neighbouring node connected to and
   * cost of reaching the destination node
   */
  boost::unordered::unordered_map<Node, double_t> neighbours;
};

} // namespace state
