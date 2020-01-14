#include "state/path_planner/graph/graph.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;

class GraphTest : public testing::Test {
protected:
  std::vector<Vec2D> nodes;
  unique_ptr<state::Graph> graph;

  GraphTest() {
    graph = make_unique<state::Graph>();

    nodes.push_back({0, 2});
    nodes.push_back({2, 4});
    nodes.push_back({4, 4});
    nodes.push_back({6, 4});
    nodes.push_back({8, 2});
    nodes.push_back({6, 0});
    nodes.push_back({4, 0});
    nodes.push_back({2, 0});
    nodes.push_back({4, 2});
    nodes.push_back({10, 10});

    for (auto node : nodes) {
      graph->AddNode(node);
    }

    graph->AddEdge(nodes[0], nodes[1], 4);
    graph->AddEdge(nodes[0], nodes[7], 8);
    graph->AddEdge(nodes[1], nodes[2], 8);
    graph->AddEdge(nodes[1], nodes[7], 11);
    graph->AddEdge(nodes[2], nodes[3], 7);
    graph->AddEdge(nodes[2], nodes[5], 4);
    graph->AddEdge(nodes[2], nodes[8], 2);
    graph->AddEdge(nodes[3], nodes[5], 14);
    graph->AddEdge(nodes[3], nodes[4], 9);
    graph->AddEdge(nodes[4], nodes[5], 10);
    graph->AddEdge(nodes[5], nodes[6], 2);
    graph->AddEdge(nodes[6], nodes[7], 1);
    graph->AddEdge(nodes[6], nodes[8], 6);
    graph->AddEdge(nodes[7], nodes[8], 7);
  }
};

TEST_F(GraphTest, GetSizeTest) { EXPECT_EQ(graph->GetNumNodes(), 10); }

TEST_F(GraphTest, CheckPositionExistsTest) {
  EXPECT_EQ(graph->CheckPositionExists(nodes[0]), true);
  EXPECT_EQ(graph->CheckPositionExists({-10, 0}), false);
}

TEST_F(GraphTest, CheckEdgeExistsTest) {
  EXPECT_EQ(graph->CheckEdgeExists(nodes[0], nodes[7]), true);
  EXPECT_EQ(graph->CheckEdgeExists(nodes[0], nodes[8]), false);
}

TEST_F(GraphTest, AddNodeTest) {
  graph->AddNode({11, 11});
  EXPECT_EQ(graph->GetNumNodes(), 11);
}

TEST_F(GraphTest, RemoveNodeTest) {
  graph->RemoveNode({11, 11});
  EXPECT_EQ(graph->GetNumNodes(), 10);
}

TEST_F(GraphTest, AddEdgeTest) {
  graph->AddEdge(nodes[0], nodes[4], 10);
  EXPECT_EQ(graph->CheckEdgeExists(nodes[0], nodes[4]), true);
}

TEST_F(GraphTest, RemoveEdgeTest) {
  graph->RemoveEdge(nodes[0], nodes[4]);
  EXPECT_EQ(graph->CheckEdgeExists(nodes[0], nodes[4]), false);
}

TEST_F(GraphTest, GetBasicPathTest) {
  std::vector<Vec2D> path = graph->GetPath(nodes[0], nodes[4]);
  EXPECT_EQ(path.size(), 4);
  EXPECT_EQ(path[0], nodes[7]);
  EXPECT_EQ(path[1], nodes[6]);
  EXPECT_EQ(path[2], nodes[5]);
  EXPECT_EQ(path[3], nodes[4]);

  std::vector<Vec2D> path2 = graph->GetPath(nodes[3], nodes[5]);
  EXPECT_EQ(path2.size(), 2);
  EXPECT_EQ(path2[0], nodes[2]);
  EXPECT_EQ(path2[1], nodes[5]);
}

TEST_F(GraphTest, GetInvalidPathTest) {
  std::vector<Vec2D> path = graph->GetPath(nodes[0], nodes[9]);
  EXPECT_EQ(path.size(), 0);
}

TEST_F(GraphTest, GetInvalidPositionPathTest) {
  std::vector<Vec2D> path = graph->GetPath(nodes[0], {10, 1});
  EXPECT_EQ(path.size(), 0);
}

TEST_F(GraphTest, GetSelfPathTest) {
  std::vector<Vec2D> path = graph->GetPath(nodes[0], nodes[0]);
  EXPECT_EQ(path.size(), 0);
}
