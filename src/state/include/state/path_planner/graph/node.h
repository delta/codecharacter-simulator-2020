/**
 * @file waypoint.h
 * Declares a wrapper for a standard graph
 */

#pragma once

#include "physics/vector.hpp"

namespace state {

struct Node {
  /**
   * Node position
   */
  Vec2D position;

  /**
   * Equality comparator for two nodes
   * @param other
   * @return
   */
  bool operator==(Node const &other) const {
    return position == other.position;
  };

  bool operator<(Node const &other) const {
    if (position.x == other.position.x) {
      return position.y < other.position.y;
    }
    return position.x < other.position.x;
  };

  bool operator>(Node const &other) const {
    if (position.x == other.position.x) {
      return position.y > other.position.y;
    }
    return position.x > other.position.x;
  };
};

std::size_t hash_value(Node const &node) { return hash_value(node.position); }

} // namespace state
