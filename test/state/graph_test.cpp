#include "state/path_planner/graph/graph.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;

class GraphTest : public testing::Test {
  protected:
    std::vector<DoubleVec2D> nodes;
    unique_ptr<state::Graph> graph;

    GraphTest() {
        graph = make_unique<state::Graph>();

        nodes.emplace_back(0, 2);
        nodes.emplace_back(2, 4);
        nodes.emplace_back(4, 4);
        nodes.emplace_back(6, 4);
        nodes.emplace_back(8, 2);
        nodes.emplace_back(6, 0);
        nodes.emplace_back(4, 0);
        nodes.emplace_back(2, 0);
        nodes.emplace_back(4, 2);
        nodes.emplace_back(10, 10);

        for (auto node : nodes) {
            graph->addNode(node);
        }

        graph->addEdge(nodes[0], nodes[1], 4);
        graph->addEdge(nodes[0], nodes[7], 8);
        graph->addEdge(nodes[1], nodes[2], 8);
        graph->addEdge(nodes[1], nodes[7], 11);
        graph->addEdge(nodes[2], nodes[3], 7);
        graph->addEdge(nodes[2], nodes[5], 4);
        graph->addEdge(nodes[2], nodes[8], 2);
        graph->addEdge(nodes[3], nodes[5], 14);
        graph->addEdge(nodes[3], nodes[4], 9);
        graph->addEdge(nodes[4], nodes[5], 10);
        graph->addEdge(nodes[5], nodes[6], 2);
        graph->addEdge(nodes[6], nodes[7], 1);
        graph->addEdge(nodes[6], nodes[8], 6);
        graph->addEdge(nodes[7], nodes[8], 7);
    }
};

TEST_F(GraphTest, GetSizeTest) { EXPECT_EQ(graph->getNumNodes(), 10); }

TEST_F(GraphTest, CheckNodeExistsTest) {
    EXPECT_EQ(graph->checkNodeExists(nodes[0]), true);
    EXPECT_EQ(graph->checkNodeExists({-10, 0}), false);
}

TEST_F(GraphTest, CheckEdgeExistsTest) {
    EXPECT_EQ(graph->checkEdgeExists(nodes[0], nodes[7]), true);
    EXPECT_EQ(graph->checkEdgeExists(nodes[0], nodes[8]), false);
}

TEST_F(GraphTest, AddNodeTest) {
    graph->addNode({11, 11});
    EXPECT_EQ(graph->getNumNodes(), 11);
}

TEST_F(GraphTest, RemoveNodeTest) {
    graph->removeNode({11, 11});
    EXPECT_EQ(graph->getNumNodes(), 10);
}

TEST_F(GraphTest, AddEdgeTest) {
    graph->addEdge(nodes[0], nodes[4], 10);
    EXPECT_EQ(graph->checkEdgeExists(nodes[0], nodes[4]), true);
}

TEST_F(GraphTest, RemoveEdgeTest) {
    graph->removeEdge(nodes[0], nodes[4]);
    EXPECT_EQ(graph->checkEdgeExists(nodes[0], nodes[4]), false);
}

TEST_F(GraphTest, GetBasicPathTest) {
    std::vector<DoubleVec2D> path = graph->getPath(nodes[0], nodes[4]);
    EXPECT_EQ(path.size(), 4);
    EXPECT_EQ(path[0], nodes[7]);
    EXPECT_EQ(path[1], nodes[6]);
    EXPECT_EQ(path[2], nodes[5]);
    EXPECT_EQ(path[3], nodes[4]);

    std::vector<DoubleVec2D> path2 = graph->getPath(nodes[3], nodes[5]);
    EXPECT_EQ(path2.size(), 2);
    EXPECT_EQ(path2[0], nodes[2]);
    EXPECT_EQ(path2[1], nodes[5]);
}

TEST_F(GraphTest, GetInvalidPathTest) {
    std::vector<DoubleVec2D> path = graph->getPath(nodes[0], nodes[9]);
    EXPECT_EQ(path.size(), 0);
}

TEST_F(GraphTest, GetInvalidPositionPathTest) {
    std::vector<DoubleVec2D> path = graph->getPath(nodes[0], {10, 1});
    EXPECT_EQ(path.size(), 0);
}

TEST_F(GraphTest, GetSelfPathTest) {
    std::vector<DoubleVec2D> path = graph->getPath(nodes[0], nodes[0]);
    EXPECT_EQ(path.size(), 0);
}
