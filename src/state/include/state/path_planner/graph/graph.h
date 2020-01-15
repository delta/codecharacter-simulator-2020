/**
 * @file graph.h
 * Declares a wrapper for a standard graph
 */

#pragma once

#include "physics/vector.hpp"
#include "state/path_planner/graph/open_list_entry.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <cstring>
#include <queue>

namespace state {
typedef std::priority_queue<std::pair<double_t, DoubleVec2D>,
                            std::vector<std::pair<double_t, DoubleVec2D>>,
                            std::greater<>>
    Heap;

typedef boost::unordered::unordered_map<DoubleVec2D, double_t> EdgeList;

class Graph {
private:
  /**
   * List of all nodes in map
   */
  boost::unordered_set<DoubleVec2D> nodes;

  /**
   * Adjacency List for the nodes in graph
   */
  boost::unordered::unordered_map<DoubleVec2D, EdgeList> adjacency_list;

  /**
   * Map of Node and corresponding open list entry
   */
  boost::unordered::unordered_map<DoubleVec2D, OpenListEntry> open_list_entries;

  /**
   * Heap containing nodes and the cost for a-star
   */
  Heap open_list_heap;

  /**
   * Initialize the openListEntries and openListHeap for a
   * given start node to destination node
   * @param start_node
   * @param destination_node
   */
  void InitOpenList(DoubleVec2D start_node,
                    const DoubleVec2D &destination_node);

  /**
   * Get the next position in the open list with smallest total cost
   * @param next_position
   * @return next node in open list
   */
  bool GetNearestNextPosition(DoubleVec2D &next_position);

  /**
   * Update the open list details of one neighbour of a node
   * @param current_node
   * @param neighbour_node
   * @param distance
   * @param destination_node Final destination
   */
  void UpdateNeighbour(DoubleVec2D current_node, DoubleVec2D neighbour_node,
                       double_t distance, const DoubleVec2D &destination_node);

public:
  /**
   * Get number of nodes in the graph
   */
  size_t GetNumNodes() const;

  /**
   * Get nodes
   */
  boost::unordered_set<DoubleVec2D> GetNodes() const;

  /**
   * Check node exists
   * @param node Node to be checked
   */
  bool CheckNodeExists(const DoubleVec2D &node) const;

  /**
   * Check if edge exists between two positions
   * @param node_a
   * @param node_b
   * @return bool true, if there is an edge between two positions
   */
  bool CheckEdgeExists(const DoubleVec2D &position_a,
                       const DoubleVec2D &position_b);

  /**
   * Add a new node to the list of nodes
   * @param position
   */
  void AddNode(DoubleVec2D position);

  /**
   * Remove a node from the list of nodes
   * @param position
   */
  void RemoveNode(DoubleVec2D position);

  /**
   * Add an edge between two positions
   * @param start_position
   * @param end_position
   * @param cost Cost of traversing from start position to end position
   */
  void AddEdge(DoubleVec2D start_position, DoubleVec2D end_position,
               double_t cost);

  /**
   * Remove an edge from the graph
   * @param start_position
   * @param end_position
   */
  void RemoveEdge(DoubleVec2D start_position, DoubleVec2D end_position);

  /**
   * Clear all graph nodes and edges
   */
  void ResetGraph();

  /**
   * Get the next node to move in the shortest path from start to end
   * @param start_position
   * @param end_position
   * @return std::vector<DoubleVec2D> next node in the path
   */
  std::vector<DoubleVec2D> GetPath(DoubleVec2D start_position,
                                   DoubleVec2D end_position);
};

} // namespace state
