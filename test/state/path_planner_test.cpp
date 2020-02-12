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
    ASSERT_EQ(path_planner->isOffsetBlocked({15, 0}), true);
    ASSERT_EQ(path_planner->isOffsetBlocked({0, 15}), true);
    ASSERT_EQ(path_planner->isOffsetBlocked({10, 10}), false);
    ASSERT_EQ(path_planner->isOffsetBlocked({0, 0}), false);
}

TEST_F(PathPlannerTest, isValidTowerPositionBoundaryPlayer1Test) {

    // Checking on map boundary for Player1
    ASSERT_EQ(path_planner->isValidTowerPosition({MAP_SIZE, MAP_SIZE},
                                                 PlayerId::PLAYER1),
              false);
    ASSERT_EQ(
        path_planner->isValidTowerPosition({0, MAP_SIZE}, PlayerId::PLAYER1),
        false);
    ASSERT_EQ(path_planner->isValidTowerPosition({0, 0}, PlayerId::PLAYER1),
              true);
    ASSERT_EQ(path_planner->isValidTowerPosition({0, 5}, PlayerId::PLAYER1),
              true);
}

TEST_F(PathPlannerTest, isValidTowerPositionWaterPlayer1Test) {

    // Checking on water
    ASSERT_EQ(
        path_planner->isValidTowerPosition({0.5, 14.5}, PlayerId::PLAYER1),
        false);
}

TEST_F(PathPlannerTest, isValidTowerPositionLandPlayer1Test) {

    // Checking on land
    ASSERT_EQ(path_planner->isValidTowerPosition({3.5, 3.5}, PlayerId::PLAYER1),
              true);
}

TEST_F(PathPlannerTest, isValidTowerPositionBoundaryPlayer2Test) {

    // Checking on map boundary for Player2
    ASSERT_EQ(path_planner->isValidTowerPosition({0, 0}, PlayerId::PLAYER2),
              false);
    ASSERT_EQ(
        path_planner->isValidTowerPosition({0, MAP_SIZE}, PlayerId::PLAYER1),
        false);
    ASSERT_EQ(path_planner->isValidTowerPosition({MAP_SIZE, MAP_SIZE},
                                                 PlayerId::PLAYER2),
              true);
    ASSERT_EQ(
        path_planner->isValidTowerPosition({MAP_SIZE, 7}, PlayerId::PLAYER2),
        true);
}

TEST_F(PathPlannerTest, isValidTowerPositionWaterPlayer2Test) {

    // Checking on water
    ASSERT_EQ(
        path_planner->isValidTowerPosition({0.5, 14.5}, PlayerId::PLAYER2),
        false);
}

TEST_F(PathPlannerTest, isValidTowerPositionLandPlayer2Test) {
    // Checking on land
    ASSERT_EQ(path_planner->isValidTowerPosition({3.5, 3.5}, PlayerId::PLAYER2),
              true);
}

TEST_F(PathPlannerTest, isValidBotPositionBoundaryTest) {

    // Checking on map boundary
    ASSERT_EQ(path_planner->isValidBotPosition({MAP_SIZE, MAP_SIZE}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({0, MAP_SIZE}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({0, 5}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({0, 0}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({MAP_SIZE, 7}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({MAP_SIZE, 7}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({0, 14.5}), false);
    ASSERT_EQ(path_planner->isValidBotPosition({0, 14}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({0, 15}), false);
}

TEST_F(PathPlannerTest, isValidBotPositionWaterTest) {

    // Checking on water
    ASSERT_EQ(path_planner->isValidBotPosition({0.5, 14.5}), false);
    ASSERT_EQ(path_planner->isValidBotPosition({0.5, 14.5}), false);
}

TEST_F(PathPlannerTest, isValidBotPositionLandTest) {

    // Checking on land
    ASSERT_EQ(path_planner->isValidBotPosition({3.5, 3.5}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({3.5, 3.5}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({1, 14.5}), false);
    ASSERT_EQ(path_planner->isValidBotPosition({14, 6}), true);
    ASSERT_EQ(path_planner->isValidBotPosition({17, 3}), true);
}

TEST_F(PathPlannerTest, AddTowerPlayer1Test) {
    // Build tower
    ASSERT_EQ(path_planner->buildTower({0, 0}, PlayerId::PLAYER1),
              DoubleVec2D(0, 0));
    ASSERT_EQ(path_planner->isOffsetBlocked({0, 0}), true);
    ASSERT_EQ(path_planner->buildTower({8.5, 8.5}, PlayerId::PLAYER1),
              DoubleVec2D(8, 8));
    ASSERT_EQ(path_planner->isOffsetBlocked({8, 8}), true);
    ASSERT_EQ(path_planner->buildTower({14, 5}, PlayerId::PLAYER1),
              DoubleVec2D::null);

    // Invalid position
    bool get_exception = false;
    try {
        path_planner->buildTower({MAP_SIZE, MAP_SIZE}, PlayerId::PLAYER1);
    } catch (...) {
        get_exception = true;
    }
    ASSERT_EQ(get_exception, true);
}

TEST_F(PathPlannerTest, AddTowerTest) {

    // Build tower
    ASSERT_EQ(path_planner->buildTower({MAP_SIZE, MAP_SIZE}, PlayerId::PLAYER2),
              DoubleVec2D(MAP_SIZE - 1, MAP_SIZE - 1));
    ASSERT_EQ(path_planner->isOffsetBlocked({MAP_SIZE - 1, MAP_SIZE - 1}),
              true);
    ASSERT_EQ(path_planner->buildTower({14, 7}, PlayerId::PLAYER2),
              DoubleVec2D(13, 6));

    // Invalid position
    bool get_exception = false;
    try {
        path_planner->buildTower({0, 0}, PlayerId::PLAYER2);
    } catch (...) {
        get_exception = true;
    }
    ASSERT_EQ(get_exception, true);
}

TEST_F(PathPlannerTest, RemoveTowerBasicTest) {
    auto tower_offset = path_planner->buildTower({0.5, 0.5}, PlayerId::PLAYER1);
    ASSERT_EQ(path_planner->destroyTower(tower_offset), true);
    ASSERT_EQ(path_planner->isOffsetBlocked(tower_offset), false);

    tower_offset =
        path_planner->buildTower({MAP_SIZE, MAP_SIZE}, PlayerId::PLAYER2);
    ASSERT_EQ(path_planner->destroyTower(tower_offset), true);
    ASSERT_EQ(path_planner->isOffsetBlocked(tower_offset), false);
}

TEST_F(PathPlannerTest, RemoveTowerInvalidTest) {
    ASSERT_EQ(path_planner->destroyTower({0, 0}), false);
    ASSERT_EQ(path_planner->destroyTower({14, 1}), false);
    ASSERT_EQ(path_planner->isOffsetBlocked({14, 1}), true);
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
