/**
 * @file node.h
 * Declares a single graph node
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
  bool operator==(Node const &rhs) const { return position == rhs.position; };

  bool operator<(Node const &rhs) const {
    if (position.x == rhs.position.x) {
      return position.y < rhs.position.y;
    }
    return position.x < rhs.position.x;
  };

  bool operator>(Node const &rhs) const {
    if (position.x == rhs.position.x) {
      return position.y > rhs.position.y;
    }
    return position.x > rhs.position.x;
  };
};

std::size_t hash_value(Node const &node) { return hash_value(node.position); }

} // namespace state
