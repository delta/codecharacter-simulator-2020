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
   * @param rhs
   * @return true, if this node is equal to rhs
   */
  bool operator==(Node const &rhs) const { return position == rhs.position; };

  /**
   * Less than comparator for two nodes
   * @param rhs
   * @return true, if this node is less than rhs
   */
  bool operator<(Node const &rhs) const {
    if (position.x == rhs.position.x) {
      return position.y < rhs.position.y;
    }
    return position.x < rhs.position.x;
  };

  /**
   * Greater than comparator for two nodes
   * @param rhs
   * @return true, if this node is greater than rhs
   */
  bool operator>(Node const &rhs) const {
    if (position.x == rhs.position.x) {
      return position.y > rhs.position.y;
    }
    return position.x > rhs.position.x;
  };
};

/**
 * Hash function to be used by boost
 * @param node
 * @return std::size_t hash value of node
 */
std::size_t hash_value(Node const &node) { return hash_value(node.position); }

} // namespace state
