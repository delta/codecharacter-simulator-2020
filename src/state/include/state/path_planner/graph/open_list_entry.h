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
  double_t g_value;

  /**
   * Heuristic cost to reach from node to destination in a-star
   */
  double_t h_value;

  /**
   * Previous node in the a-star path generated
   */
  Node parent;

  /**
   * Track whether the node is open
   */
  bool is_open;

  /**
   * Get sum of gValue and hValue
   * @return total value
   */
  double_t getTotalValue() const { return gValue + hValue; }

  /**
   * Equality comparator for two open list nodes
   * @param rhs
   * @return
   */
  bool operator==(OpenListEntry const &rhs) const {
    return (g_value + h_value) == (rhs.g_value + rhs.h_value);
  };

  bool operator<(const OpenListEntry &rhs) const {
    return (g_value + h_value) < (rhs.g_value + rhs.h_value);
  };

  bool operator>(const OpenListEntry &rhs) const {
    return (g_value + h_value) > (rhs.g_value + rhs.h_value);
  };
};

} // namespace state
