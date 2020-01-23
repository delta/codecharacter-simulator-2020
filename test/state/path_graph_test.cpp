#include "state/path_planner/path_graph.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace state;

const auto MAP_SIZE = size_t{10};

const auto T = true;
const auto F = false;

class WaypointGraphTest : public testing::Test {
  protected:
    vector<vector<bool>> valid_terrain;
    unique_ptr<PathGraph> waypointGraph;

    WaypointGraphTest() {
        valid_terrain = std::vector<std::vector<bool>>(
            MAP_SIZE, std::vector<bool>(MAP_SIZE, true));
        waypointGraph =
            make_unique<PathGraph>(MAP_SIZE, valid_terrain, Graph());

        for (double_t i = 1; i <= 7; i++)
            waypointGraph->addObstacle({2, i});
        for (double_t i = 3; i <= 7; i++)
            waypointGraph->addObstacle({i, 7});
        for (double_t i = 3; i <= 7; i++)
            waypointGraph->addObstacle({i, 4});

        waypointGraph->recomputeWaypointGraph();
    }
};

TEST_F(WaypointGraphTest, FMapTest1) {
    auto path = waypointGraph->getPath({5, 6}, {0, 4});
    ASSERT_EQ(path.size(), 5);
    ASSERT_EQ(path[0], DoubleVec2D(8, 5));
    ASSERT_EQ(path[1], DoubleVec2D(8, 4));
    ASSERT_EQ(path[2], DoubleVec2D(3, 1));
    ASSERT_EQ(path[3], DoubleVec2D(2, 1));
    ASSERT_EQ(path[4], DoubleVec2D(0, 4));
}

TEST_F(WaypointGraphTest, FMapTest2) {
    auto path = waypointGraph->getPath({5, 6}, {0, 7});
    ASSERT_EQ(path.size(), 4);
    ASSERT_EQ(path[0], DoubleVec2D(8, 7));
    ASSERT_EQ(path[1], DoubleVec2D(8, 8));
    ASSERT_EQ(path[2], DoubleVec2D(2, 8));
    ASSERT_EQ(path[3], DoubleVec2D(0, 7));
}

TEST_F(WaypointGraphTest, FMapTest3) {
    auto path = waypointGraph->getPath({0, 4}, {0, 7});
    ASSERT_EQ(path.size(), 1);
    ASSERT_EQ(path[0], DoubleVec2D(0, 7));
}

TEST_F(WaypointGraphTest, FMapTest4) {
    auto path = waypointGraph->getPath({0, 0}, {9, 0});
    ASSERT_EQ(path.size(), 1);
    ASSERT_EQ(path[0], DoubleVec2D(9, 0));
}

TEST_F(WaypointGraphTest, FMapTest5) {
    auto path = waypointGraph->getPath({5.5, 5.5}, {6.5, 6.5});
    ASSERT_EQ(path.size(), 1);
    ASSERT_EQ(path[0], DoubleVec2D(6.5, 6.5));
}

TEST_F(WaypointGraphTest, UnreachablityTest) {
    waypointGraph->addObstacle({7, 5});
    waypointGraph->addObstacle({7, 6});

    waypointGraph->recomputeWaypointGraph();

    auto path = waypointGraph->getPath({5, 6}, {0, 7});
    ASSERT_EQ(path.size(), 0);

    waypointGraph->removeObstacle({7, 5});
    waypointGraph->removeObstacle({7, 6});

    waypointGraph->recomputeWaypointGraph();
}

TEST_F(WaypointGraphTest, InvalidStartTest) {
    auto path = waypointGraph->getPath({2.5, 2}, {0, 7});
    ASSERT_EQ(path.size(), 0);
}

TEST_F(WaypointGraphTest, InvalidDestinationTest) {
    auto path = waypointGraph->getPath({0, 7}, {2.5, 1.5});
    ASSERT_EQ(path.size(), 0);
}

TEST_F(WaypointGraphTest, AlongWaypointTest) {
    auto path = waypointGraph->getPath({3, 1}, {8, 4});
    ASSERT_EQ(path.size(), 1);
}

TEST_F(WaypointGraphTest, LongPathTest) {
    auto test_terrain = valid_terrain;
    for (auto i = 0; i <= 8; i++)
        test_terrain[2][i] = false;
    for (auto i = 1; i <= 9; i++)
        test_terrain[5][i] = false;
    for (auto i = 0; i <= 8; i++)
        test_terrain[8][i] = false;

    waypointGraph->setValidTerrain(test_terrain);

    auto path = waypointGraph->getPath({0, 0}, {9.5, 0.5});
    ASSERT_EQ(path.size(), 7);
    ASSERT_EQ(path[0], DoubleVec2D(2, 9));
    ASSERT_EQ(path[1], DoubleVec2D(3, 9));
    ASSERT_EQ(path[2], DoubleVec2D(5, 1));
    ASSERT_EQ(path[3], DoubleVec2D(6, 1));
    ASSERT_EQ(path[4], DoubleVec2D(8, 9));
    ASSERT_EQ(path[5], DoubleVec2D(9, 9));
    ASSERT_EQ(path[6], DoubleVec2D(9.5, 0.5));
}

TEST_F(WaypointGraphTest, SpiralPathTest) {
    std::vector<std::vector<bool>> test_terrain{
        {F, T, T, T, T, T, T, T, T, T}, {F, T, F, F, F, F, F, F, T, T},
        {F, T, F, T, T, T, T, F, T, T}, {F, T, F, T, F, F, T, F, T, T},
        {F, T, F, T, T, F, T, F, T, T}, {F, T, F, T, T, F, T, F, T, T},
        {F, T, F, F, F, F, T, F, T, T}, {F, T, T, T, T, T, T, F, T, T},
        {F, F, F, F, F, F, F, F, T, T}, {T, T, T, T, T, T, T, T, T, T}};

    waypointGraph->setValidTerrain(test_terrain);

    auto path = waypointGraph->getPath({4.5, 4.5}, {9.5, 0.5});

    ASSERT_EQ(path.size(), 9);
    ASSERT_EQ(path[0], DoubleVec2D(4, 4));
    ASSERT_EQ(path[1], DoubleVec2D(3, 4));
    ASSERT_EQ(path[2], DoubleVec2D(3, 6));
    ASSERT_EQ(path[3], DoubleVec2D(7, 6));
    ASSERT_EQ(path[4], DoubleVec2D(7, 2));
    ASSERT_EQ(path[5], DoubleVec2D(1, 2));
    ASSERT_EQ(path[6], DoubleVec2D(1, 8));
    ASSERT_EQ(path[7], DoubleVec2D(9, 8));
    ASSERT_EQ(path[8], DoubleVec2D(9.5, 0.5));
}
