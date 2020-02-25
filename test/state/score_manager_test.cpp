#include "state/actor/bot.h"
#include "state/map/map.h"
#include "state/path_planner/path_planner.h"
#include "state/score_manager/score_manager.h"
#include "state/utilities.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace testing;

class ScoreManagerTest : public Test {
  public:
    array<vector<Bot *>, 2> bots;
    unique_ptr<ScoreManager> score_manager;
    unique_ptr<PathPlanner> path_planner;
    size_t map_size;

    ScoreManagerTest() {
        // Creating player bots
        score_manager = make_unique<ScoreManager>();

        // Creating a map
        vector<vector<TerrainType>> test_map;
        const TerrainType L = TerrainType::LAND;
        const TerrainType F = TerrainType::FLAG;

        map_size = 10;

        /**
         *
         * B1 - Bots of player1
         * B2 - Bots of player2
         *
         *
         * L L L L L L L L L L
         * L L L L L L L L L L
         * L L L L L L L L L L
         * L L L L B2 L L L L L
         * F F F F F F F F F F
         * F F F F F F F F F F
         * L L L L B1 L L L L L
         * L L L L L L L L L L
         * L L L L L L L L L L
         * L L L L L L L L L L
         *
         */

        for (size_t x = 0; x < map_size; ++x) {
            vector<TerrainType> map_row;
            for (size_t y = 0; y < map_size; ++y) {
                if (y == (map_size / 2) || y == (map_size / 2) - 1) {
                    map_row.push_back(F);
                } else {
                    map_row.push_back(L);
                }
            }
            test_map.push_back(map_row);
        }

        // Creating bots for each player
        auto speed = 5;
        auto map = make_unique<Map>(test_map, map_size);
        path_planner = make_unique<PathPlanner>(std::move(map));
        for (int64_t bot_index = 0; bot_index < 5; ++bot_index) {
            auto bot = new state::Bot(
                Actor::getNextActorId(), PlayerId::PLAYER1, 100, 100,
                DoubleVec2D(5, 7), speed, 1, 1, score_manager.get(),
                path_planner.get(), BlastCallback{}, ConstructTowerCallback{});
            bots[0].push_back(bot);
        }

        for (int64_t bot_index = 0; bot_index < 5; ++bot_index) {
            auto bot = new state::Bot(
                Actor::getNextActorId(), PlayerId::PLAYER2, 100, 100,
                DoubleVec2D(5, 3), speed, 1, 1, score_manager.get(),
                path_planner.get(), BlastCallback{}, ConstructTowerCallback{});
            bots[1].push_back(bot);
        }
    }
};

TEST_F(ScoreManagerTest, BotEnterExitTest) {
    // Setting 2 bots of PLAYER1 and 1 bot of PLAYER2 to move into flag area
    bots[0][0]->setDestination(DoubleVec2D(5, 5));
    bots[0][1]->setDestination(DoubleVec2D(5, 5));
    bots[1][0]->setDestination(DoubleVec2D(5, 5));

    bots[0][0]->update();
    bots[0][1]->update();
    bots[1][0]->update();

    bots[0][0]->lateUpdate();
    bots[0][1]->lateUpdate();
    bots[1][0]->lateUpdate();

    ASSERT_EQ(bots[0][0]->getPosition(), DoubleVec2D(5, 5));
    ASSERT_EQ(bots[0][1]->getPosition(), DoubleVec2D(5, 5));
    ASSERT_EQ(bots[1][0]->getPosition(), DoubleVec2D(5, 5));

    auto tower_counts = score_manager->getTowerCounts();
    auto bot_counts = score_manager->getBotCounts();

    ASSERT_EQ(tower_counts[0], 0);
    ASSERT_EQ(tower_counts[1], 0);
    ASSERT_EQ(bot_counts[0], 2);
    ASSERT_EQ(bot_counts[1], 1);

    score_manager->updateScores();
    auto scores = score_manager->getScores();

    ASSERT_EQ(scores[0], 1);
    ASSERT_EQ(scores[1], 0);

    // Making the PLAYER1 bots leave the flag area
    bots[0][0]->setDestination(DoubleVec2D(5, 3));
    bots[0][1]->setDestination(DoubleVec2D(5, 3));

    bots[0][0]->update();
    bots[0][1]->update();

    bots[0][0]->lateUpdate();
    bots[0][1]->lateUpdate();

    bot_counts = score_manager->getBotCounts();
    ASSERT_EQ(bot_counts[0], 0);
    ASSERT_EQ(bot_counts[1], 1);

    score_manager->updateScores();
    scores = score_manager->getScores();

    ASSERT_EQ(scores[0], 1);
    ASSERT_EQ(scores[0], 1);
}

TEST_F(ScoreManagerTest, BotDyingTest) {
    // Moving 4 PLAYER1 and PLAYER2 bots into flag area
    for (int64_t player_id = 0; player_id < 2; ++player_id) {
        for (int64_t bot_index = 0; bot_index < 4; ++bot_index) {
            bots[player_id][bot_index]->setDestination(DoubleVec2D(5, 5));
            bots[player_id][bot_index]->update();
            bots[player_id][bot_index]->lateUpdate();
            ASSERT_EQ(bots[player_id][bot_index]->getPosition(),
                      DoubleVec2D(5, 5));
        }
    }

    auto bot_counts = score_manager->getBotCounts();
    ASSERT_EQ(bot_counts[0], 4);
    ASSERT_EQ(bot_counts[1], 4);

    // Transforming all PLAYER2 bots into dead state
    for (int64_t bot_index = 0; bot_index < 4; ++bot_index) {
        bots[1][bot_index]->setHp(0);
        bots[1][bot_index]->update();
        bots[1][bot_index]->lateUpdate();
        ASSERT_EQ(bots[1][bot_index]->getState(), BotStateName::DEAD);
    }

    bot_counts = score_manager->getBotCounts();
    ASSERT_EQ(bot_counts[0], 4);
    ASSERT_EQ(bot_counts[1], 0);

    score_manager->updateScores();
    auto scores = score_manager->getScores();
    ASSERT_EQ(scores[0], 1);
    ASSERT_EQ(scores[1], 0);
}

// TODO : Add test for bot transforming into tower after implementation of state
