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
   * @param node Node to be checked
   */
  bool CheckNodeExists(const Node &node) const;

  /**
   * Initialize the openListEntries and openListHeap for a
   * given start node to destination node
   * @param start_node
   * @param destination_node
   */
  void InitOpenList(Node start_node, const Node &destination_node);

  /**
   * Get the position in the open list with smallest total cost
   * @param next_position
   * @return next node in open list
   */
  bool GetSmallestNextPosition(Vec2D &next_position);

  /**
   * Update the open list details of one neighbour of a node
   * @param current_node
   * @param neighbour_node
   * @param distance
   * @param destination_node Final destination
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
   * @param position Position to be checked
   * @return bool true, if given position is one of the nodes
   */
  bool CheckPositionExists(const Vec2D &position) const;

  /**
   * Check if edge exists between two positions
   * @param node_a
   * @param node_b
   * @return bool true, if there is an edge between two positions
   */
  bool CheckEdgeExists(const Vec2D &position_a, const Vec2D &position_b);

  /**
   * Add a new node to the list of nodes
   * @param position
   */
  void AddNode(Vec2D position);

  /**
   * Remove a node from the list of nodes
   * @param position
   */
  void RemoveNode(Vec2D position);

  /**
   * Add an edge between two positions
   * @param start_position
   * @param end_position
   * @param cost Cost of traversing from start position to end position
   */
  void AddEdge(Vec2D start_position, Vec2D end_position, double_t cost);

  /**
   * Remove an edge from the graph
   * @param start_position
   * @param end_position
   */
  void RemoveEdge(Vec2D start_position, Vec2D end_position);

  /**
   * Get the next node to move in the shortest path from start to end
   * @param start_position
   * @param end_position
   * @return std::vector<Vec2D> next node in the path
   */
  std::vector<Vec2D> GetPath(Vec2D start_position, Vec2D end_position);
};

} // namespace state
