/**
 * @file waypoint.cpp
 * Defines the Waypoint Graph class
 */

#include "state/path_planner/graph/graph.h"

namespace state {

Graph::Graph() {}

size_t Graph::GetNumNodes() const { return nodes.size(); }

bool Graph::CheckNodeExists(const Node &node) const {
  return (nodes.find(node) != nodes.end());
}

bool Graph::CheckPositionExists(const Vec2D &position) const {
  return CheckNodeExists({position});
}

bool Graph::CheckEdgeExists(const Vec2D &position_a, const Vec2D &position_b) {
  Node node_a = {position_a};
  Node node_b = {position_b};

  if (CheckPositionExists(position_a) && CheckPositionExists(position_b)) {
    if (adjacency_list[node_a].neighbours.find(node_b) !=
        adjacency_list[node_a].neighbours.end()) {
      return true;
    }

    return false;
  }

  return false;
}

void Graph::AddNode(Vec2D position) {
  Node new_node = {position};

  // Insert the node only if it doesn't exist
  if (!CheckNodeExists(new_node)) {
    NodeEdgeList edge_list;
    edge_list.node = new_node;
    adjacency_list.insert({new_node, edge_list});

    nodes.insert(new_node);
  }
}

void Graph::RemoveNode(Vec2D position) {
  Node node = {position};

  if (CheckNodeExists(node)) {
    nodes.erase(node);
    adjacency_list.erase(node);
  }
}

void Graph::AddEdge(Vec2D start_position, Vec2D end_position, double_t cost) {
  Node start_node = {start_position};
  Node end_node = {end_position};

  if (cost < 0)
    return;

  if (CheckNodeExists(start_node)) {
    adjacency_list[start_node].neighbours.insert({end_node, cost});
  }

  if (CheckNodeExists(end_node)) {
    adjacency_list[end_node].neighbours.insert({start_node, cost});
  }
}

void Graph::RemoveEdge(Vec2D start_position, Vec2D end_position) {
  Node start_node = {start_position};
  Node end_node = {end_position};

  if (CheckNodeExists(start_node)) {
    adjacency_list[start_node].neighbours.erase(end_node);
  }

  if (CheckNodeExists(end_node)) {
    adjacency_list[end_node].neighbours.erase(start_node);
  }
}

void Graph::InitOpenList(Node start_node, const Node &destination_node) {
  open_list_entries.clear();
  open_list_heap = Heap();

  // Creating an open list entry for the start node
  OpenListEntry start_node_entry;

  // Cost from start node to itself is 0
  start_node_entry.gValue = 0;

  // Heuristic cost from start node to end node
  start_node_entry.hValue =
      start_node.position.distance(destination_node.position);

  start_node_entry.parent = {Vec2D::null};
  start_node_entry.is_open = true;

  open_list_entries[start_node] = start_node_entry;
  open_list_heap.push({start_node_entry.getTotalValue(), start_node});
}

bool Graph::GetSmallestNextPosition(Vec2D &next_position) {
  if (!open_list_heap.empty()) {
    // Top node in heap has the least total cost
    auto next_node = (open_list_heap.top()).second;
    next_position = next_node.position;

    open_list_heap.pop();

    return true;
  }

  // Heap is empty
  return false;
}

void Graph::UpdateNeighbour(Node current_node, Node neighbour_node,
                            double_t distance, const Node &destination_node) {
  // Neighbour's cost from start is cost of current node + distance between
  // current node and neighbour
  double_t neighbour_g_value =
      open_list_entries[current_node].gValue + distance;

  // Neighbour's heuristic cost to destination is euclidian distance between
  // them
  double_t neighbour_h_value =
      neighbour_node.position.distance(destination_node.position);

  // Neighbour node has not been visited yet
  if (open_list_entries.find(neighbour_node) == open_list_entries.end()) {

    open_list_entries[neighbour_node] =
        OpenListEntry{neighbour_g_value, neighbour_h_value, current_node, true};
    open_list_heap.push(
        {neighbour_g_value + neighbour_h_value, neighbour_node});
  } else {
    auto neighbour_open_list_node =
        &(open_list_entries.find(neighbour_node)->second);

    // Neighbour node is not closed, if closed, ignore
    if (neighbour_open_list_node->is_open) {
      auto current_total_value = neighbour_open_list_node->getTotalValue();

      // Update only if new total cost is less than previous total
      // cost
      if (current_total_value > (neighbour_g_value + neighbour_h_value)) {

        neighbour_open_list_node->parent = current_node;
        neighbour_open_list_node->gValue = neighbour_g_value;
        neighbour_open_list_node->hValue = neighbour_h_value;

        open_list_heap.push(
            {neighbour_g_value + neighbour_h_value, neighbour_node});
      }
    }
  }
}

std::vector<Vec2D> Graph::GetPath(Vec2D start_position, Vec2D end_position) {
  Node start_node = {start_position};
  Node end_node = {end_position};

  if (!CheckPositionExists(start_position) ||
      !CheckPositionExists(end_position)) {
    return std::vector<Vec2D>{};
  }

  if (start_position == end_position) {
    return std::vector<Vec2D>{};
  }

  InitOpenList(start_node, end_node);

  // Current position while traversing through graph
  auto current_position = Vec2D::null;

  while (GetSmallestNextPosition(current_position)) {
    if (!open_list_entries[{current_position}].is_open)
      continue;

    open_list_entries[{current_position}].is_open = false;

    // Return path
    if (current_position == end_position) {
      auto result = std::vector<Vec2D>{};
      auto result_position = current_position;

      // Traceback through the nodes' parents to get the complete path
      while (result_position &&
             open_list_entries[{result_position}].parent.position !=
                 Vec2D::null) {
        result.push_back(result_position);
        result_position = open_list_entries[{result_position}].parent.position;
      }

      reverse(result.begin(), result.end());
      return result;
    }

    // Add neighbours to openListHeap and updateOpenListEntries
    for (auto neighbour : adjacency_list[{current_position}].neighbours) {
      UpdateNeighbour({current_position}, neighbour.first, neighbour.second,
                      end_node);
    }
  }

  return std::vector<Vec2D>{};
}

} // namespace state
