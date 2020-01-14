/**
 * @file graph.h
 * Declares a wrapper for a standard graph
 */

#pragma once

#include "physics/vector.hpp"
#include "state/path_planner/graph/node.h"
#include "state/path_planner/graph/node_edge_list.h"
#include "state/path_planner/graph/open_list_entry.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <cstring>
#include <queue>

namespace state {
typedef std::priority_queue<std::pair<double_t, Node>,
                            std::vector<std::pair<double_t, Node>>,
                            std::greater<>>
    Heap;

class Graph {
private:
  /**
   * List of all nodes in map
   */
  boost::unordered_set<Node> nodes;

  /**
   * Adjacency List for the nodes in graph
   */
  boost::unordered::unordered_map<Node, NodeEdgeList> adjacency_list;

  /**
   * Map of Node and corresponding open list entry
   */
  boost::unordered::unordered_map<Node, OpenListEntry> open_list_entries;

  /**
   * Heap containing nodes and the cost for a-star
   */
  Heap open_list_heap;

  /**
   * Check node exists
   */
  bool CheckNodeExists(const Node &node) const;

  /**
   * Initialize the openListEntries and openListHeap
   * @param start_node
   * @param end_node
   */
  void InitOpenList(Node start_node, const Node &destination_node);

  /**
   * Get the node in the open list with smallest total cost
   * @param node
   * @return next node in open list
   */
  bool GetSmallestNextPosition(Vec2D &next_position);

  /**
   * Update the open list details of a neighbours
   * @param current_node
   * @param neighbour_node
   * @param distance
   * @param destination_node
   */
  void UpdateNeighbour(Node current_node, Node neighbour_node,
                       double_t distance, const Node &destination_node);

public:
  Graph();

  /**
   * Get number of nodes in the graph
   */
  size_t GetNumNodes() const;

  /**
   * Check position exists
   */
  bool CheckPositionExists(const Vec2D &position) const;

  /**
   * Check if edge exists between two positions
   */
  bool CheckEdgeExists(const Vec2D &node_a, const Vec2D &node_b);

  /**
   * Add a new node to the list of nodes
   */
  void AddNode(Vec2D position);

  /**
   * Remove a node from the list of nodes
   */
  void RemoveNode(Vec2D position);

  /**
   * Add an edge between two positions
   */
  void AddEdge(Vec2D start, Vec2D end, double_t cost);

  /**
   * Remove an edge from the graph
   */
  void RemoveEdge(Vec2D start, Vec2D end);

  /**
   * Get the next node to move in the shortest path from start to end
   * @param start_position
   * @param end_position
   * @return next node in the path
   */
  std::vector<Vec2D> GetPath(Vec2D start_position, Vec2D end_position);
};

} // namespace state
