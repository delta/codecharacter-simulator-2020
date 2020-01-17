/**
 * @file open_list_entry.h
 * Declares an entry in the open list for a-star
 */

#pragma once

#include "physics/vector.hpp"

namespace state {

struct OpenListEntry {
  /**
   * Cost to reach from source to node in a-star
   */
  double_t g_value{0};

  /**
   * Heuristic cost to reach from node to destination in a-star
   * It is the euclidean distance between them
   */
  double_t h_value{0};

  /**
   * Previous node in the a-star path generated
   */
  DoubleVec2D parent;

  /**
   * Track whether the node is open
   */
  bool is_open{true};

  /**
   * Get sum of gValue and hValue
   * @return double_t total cost
   */
  double_t getTotalCost() const { return g_value + h_value; }

  /**
   * Equality comparator for two open list nodes
   * Used by priority_queue
   * @param rhs
   * @return bool true, if the cost of both nodes are equals
   */
  bool operator==(OpenListEntry const &rhs) const {
    return getTotalCost() == rhs.getTotalCost();
  };

  /**
   * Less than comparator for two open list nodes
   * Used by priority_queue
   * @param rhs
   * @return bool true, if the cost of this node is less than rhs
   */
  bool operator<(const OpenListEntry &rhs) const {
    return getTotalCost() < rhs.getTotalCost();
  };

  /**
   * Greater than comparator for two open list nodes
   * Used by priority_queue
   * @param rhs
   * @return bool true, if the cost of this node is greater than rhs
   */
  bool operator>(const OpenListEntry &rhs) const {
    return getTotalCost() > rhs.getTotalCost();
  };
};

} // namespace state
