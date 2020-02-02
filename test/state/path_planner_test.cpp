#include "state/path_planner/path_planner.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;
using namespace state;

const size_t MAP_SIZE = 20;

const auto L = state::TerrainType::LAND;
const auto W = state::TerrainType::WATER;
const auto F = state::TerrainType::FLAG;

class PathPlannerTest : public testing::Test {
  protected:
    unique_ptr<PathPlanner> path_planner;

    PathPlannerTest() {
        unique_ptr<Map> map;
        auto map_matrix = vector<vector<state::TerrainType>>{
            {{L, L, L, L, L, L, L, L, L, L, L, L, L, L, W, W, W, L, L, L},
             {L, L, L, L, L, L, L, L, L, L, L, L, L, L, W, W, W, L, L, L},
             {L, L, L, L, L, L, L, L, L, L, L, L, L, L, W, W, W, L, L, L},
             {L, L, L, L, L, L, L, L, L, L, L, L, L, L, W, W, W, W, W, W},
             {L, L, L, L, L, L, L, L, L, L, L, L, L, L, W, W, W, W, W, W},
             {L, L, L, L, L, L, L, L, W, W, W, W, W, W, W, W, W, W, W, W},
             {L, L, L, L, L, L, L, L, L, L, L, L, L, L, W, L, L, L, L, L},
             {L, L, L, L, L, L, L, F, F, F, F, F, F, L, W, L, L, L, L, L},
             {L, L, L, L, L, W, L, F, F, F, F, F, F, L, W, L, L, L, L, L},
             {L, L, L, L, L, W, L, F, F, F, F, F, F, L, W, L, L, L, L, L},
             {L, L, L, L, L, W, L, F, F, F, F, F, F, L, W, L, L, L, L, L},
             {L, L, L, L, L, W, L, F, F, F, F, F, F, L, W, L, L, L, L, L},
             {L, L, L, L, L, W, L, F, F, F, F, F, F, L, L, L, L, L, L, L},
             {L, L, L, L, L, W, L, L, L, L, L, L, L, L, L, L, L, L, L, L},
             {W, W, W, W, W, W, W, W, W, W, W, W, L, L, L, L, L, L, L, L},
             {W, W, W, W, W, W, L, L, L, L, L, L, L, L, L, L, L, L, L, L},
             {W, W, W, W, W, W, L, L, L, L, L, L, L, L, L, L, L, L, L, L},
             {L, L, L, W, W, W, L, L, L, L, L, L, L, L, L, L, L, L, L, L},
             {L, L, L, W, W, W, L, L, L, L, L, L, L, L, L, L, L, L, L, L},
             {L, L, L, W, W, W, L, L, L, L, L, L, L, L, L, L, L, L, L, L}}};

        map = make_unique<Map>(map_matrix, MAP_SIZE);
        path_planner = make_unique<PathPlanner>(std::move(map));
        path_planner->recomputePathGraph();
    }
};

TEST_F(PathPlannerTest, IsPositionBlockedTest) {
    ASSERT_EQ(path_planner->isPositionBlocked({15, 0}), true);
    ASSERT_EQ(path_planner->isPositionBlocked({0, 15}), true);
    ASSERT_EQ(path_planner->isPositionBlocked({20, 20}), false);
    ASSERT_EQ(path_planner->isPositionBlocked({10, 10}), false);
    ASSERT_EQ(path_planner->isPositionBlocked({0, 0}), false);
}

TEST_F(PathPlannerTest, AddTowerTest) {
    path_planner->addTower({8.5, 8.5});
    ASSERT_EQ(path_planner->isPositionBlocked({8.2, 8.2}), true);

    // Building a tower at invalid position, should not construct a tower
    bool get_exception = false;
    try {
        path_planner->addTower({20, 20});
    } catch (...) {
        get_exception = true;
    }
    ASSERT_EQ(get_exception, true);

    // Building tower on water
    get_exception = false;
    try {
        path_planner->addTower({14.5, 0.5});
    } catch (...) {
        get_exception = true;
    }
    ASSERT_EQ(get_exception, true);
}

TEST_F(PathPlannerTest, RemoveTowerTest) {
    path_planner->addTower({8.5, 8.5});
    path_planner->removeTower({8, 8});
    ASSERT_EQ(path_planner->isPositionBlocked({8.2, 8.2}), false);

    // Removing a tower where there is no obstacle
    bool get_exception = false;
    try {
        path_planner->removeTower({3.5, 3.5});
    } catch (...) {
        get_exception = true;
    }
    ASSERT_EQ(get_exception, true);

    // Removing water as an obstacle
    get_exception = false;
    try {
        path_planner->removeTower({0.5, 14.5});
    } catch (...) {
        get_exception = true;
    }
    ASSERT_EQ(get_exception, true);
}

TEST_F(PathPlannerTest, GetNextPositionTest) {
    DoubleVec2D start = {0.5, 0.5};
    DoubleVec2D end = {15.5, 6.5};
    size_t speed = 1;

    DoubleVec2D current_position = start;

    while (current_position != end) {
        current_position =
            path_planner->getNextPosition(current_position, end, speed);
    }

    ASSERT_EQ(current_position, end);
}
