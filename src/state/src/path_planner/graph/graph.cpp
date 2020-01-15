/**
 * @file graph.cpp
 * Defines the Graph class
 */

#include "state/path_planner/graph/graph.h"
#include <algorithm>

namespace state {

size_t Graph::GetNumNodes() const { return nodes.size(); }

boost::unordered_set<DoubleVec2D> Graph::GetNodes() const { return nodes; }

bool Graph::CheckNodeExists(const DoubleVec2D &node) const {
  return (nodes.find(node) != nodes.end());
}

bool Graph::CheckEdgeExists(const DoubleVec2D &node_a,
                            const DoubleVec2D &node_b) {
  if (CheckNodeExists(node_a) && CheckNodeExists(node_b)) {
    return adjacency_list[node_a].find(node_b) != adjacency_list[node_a].end();
  }

  return false;
}

void Graph::AddNode(DoubleVec2D node) {
  // Insert the node only if it doesn't exist
  if (!CheckNodeExists(node)) {
    EdgeList edge_list;
    adjacency_list.insert({node, edge_list});

    nodes.insert(node);
  }
}

void Graph::RemoveNode(DoubleVec2D node) {
  if (CheckNodeExists(node)) {
    nodes.erase(node);
    adjacency_list.erase(node);
  }
}

void Graph::AddEdge(DoubleVec2D start_node, DoubleVec2D end_node,
                    double_t cost) {
  if (cost < 0)
    return;

  if (CheckNodeExists(start_node)) {
    adjacency_list[start_node].insert({end_node, cost});
  }

  if (CheckNodeExists(end_node)) {
    adjacency_list[end_node].insert({start_node, cost});
  }
}

void Graph::RemoveEdge(DoubleVec2D start_node, DoubleVec2D end_node) {
  if (CheckNodeExists(start_node)) {
    adjacency_list[start_node].erase(end_node);
  }

  if (CheckNodeExists(end_node)) {
    adjacency_list[end_node].erase(start_node);
  }
}

void Graph::ResetGraph() {
  nodes.clear();
  adjacency_list.clear();
  open_list_entries.clear();
  open_list_heap = Heap();
}

void Graph::InitOpenList(DoubleVec2D start_node,
                         const DoubleVec2D &destination_node) {
  open_list_entries.clear();
  open_list_heap = Heap();

  // Creating an open list entry for the start node
  OpenListEntry start_node_entry;

  // Cost from start node to itself is 0
  start_node_entry.g_value = 0;

  // Heuristic cost from start node to end node
  start_node_entry.h_value = start_node.distance(destination_node);

  start_node_entry.parent = {DoubleVec2D::null};
  start_node_entry.is_open = true;

  open_list_entries[start_node] = start_node_entry;
  open_list_heap.push({start_node_entry.getTotalValue(), start_node});
}

bool Graph::GetNearestNextPosition(DoubleVec2D &next_position) {
  if (!open_list_heap.empty()) {
    // Top node in heap has the least total cost
    auto next_node = (open_list_heap.top()).second;
    next_position = next_node;

    open_list_heap.pop();

    return true;
  }

  // Heap is empty
  return false;
}

void Graph::UpdateNeighbour(DoubleVec2D current_node,
                            DoubleVec2D neighbour_node, double_t distance,
                            const DoubleVec2D &destination_node) {
  // Neighbour's cost from start is cost of current node + distance between
  // current node and neighbour
  double_t neighbour_g_value =
      open_list_entries[current_node].g_value + distance;

  // Neighbour's heuristic cost to destination is euclidean distance between
  // them
  double_t neighbour_h_value = neighbour_node.distance(destination_node);

  // Neighbour node has not been visited yet
  if (open_list_entries.find(neighbour_node) == open_list_entries.end()) {

    open_list_entries[neighbour_node] =
        OpenListEntry{neighbour_g_value, neighbour_h_value, current_node, true};
    open_list_heap.push(
        {neighbour_g_value + neighbour_h_value, neighbour_node});
  } else {
    auto neighbour_open_list_entry = &(open_list_entries[neighbour_node]);

    // Neighbour node is not closed, if closed, ignore
    if (neighbour_open_list_entry->is_open) {
      auto current_total_value = neighbour_open_list_entry->getTotalValue();

      // Update only if new total cost is less than previous total
      // cost
      if (current_total_value > (neighbour_g_value + neighbour_h_value)) {

        neighbour_open_list_entry->parent = current_node;
        neighbour_open_list_entry->g_value = neighbour_g_value;
        neighbour_open_list_entry->h_value = neighbour_h_value;

        open_list_heap.push(
            {neighbour_g_value + neighbour_h_value, neighbour_node});
      }
    }
  }
}

std::vector<DoubleVec2D> Graph::GetPath(DoubleVec2D start_node,
                                        DoubleVec2D end_node) {
  if (!CheckNodeExists(start_node) || !CheckNodeExists(end_node)) {
    return std::vector<DoubleVec2D>{};
  }

  if (start_node == end_node) {
    return std::vector<DoubleVec2D>{};
  }

  InitOpenList(start_node, end_node);

  // Current position while traversing through graph
  auto current_node = DoubleVec2D::null;

  while (GetNearestNextPosition(current_node)) {
    if (!open_list_entries[current_node].is_open)
      continue;

    open_list_entries[current_node].is_open = false;

    // Return path
    if (current_node == end_node) {
      auto result = std::vector<DoubleVec2D>{};
      auto result_node = current_node;

      // Traceback through the nodes' parents to get the complete path
      while (result_node &&
             open_list_entries[result_node].parent != DoubleVec2D::null) {
        result.push_back(result_node);
        result_node = open_list_entries[result_node].parent;
      }

      std::reverse(result.begin(), result.end());
      return result;
    }

    // Add neighbours to openListHeap and updateOpenListEntries
    for (auto neighbour : adjacency_list[current_node]) {
      UpdateNeighbour(current_node, neighbour.first, neighbour.second,
                      end_node);
    }
  }

  return std::vector<DoubleVec2D>{};
}

} // namespace state
