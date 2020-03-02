#include "state/player_state.h"
#include "gtest/gtest.h"

using namespace std;
using namespace state;
using namespace testing;
using namespace player_state;

const auto L = player_state::TerrainType::LAND;
const auto W = player_state::TerrainType::WATER;
const auto F = player_state::TerrainType::FLAG;

class PlayerStateTest : public Test {
  public:
    array<State, 2> player_states;
    array<array<MapElement, MAP_SIZE>, MAP_SIZE> player_map;

    PlayerStateTest() {
        // Creating a basic map and player state
        vector<vector<player_state::TerrainType>> map = {{L, L, L, L, L},
                                                         {L, W, F, W, L},
                                                         {L, F, F, F, L},
                                                         {L, W, F, W, L},
                                                         {L, L, L, L, L}};

        // Creating a map of type MapElement from terrain
        for (int x = 0; x < MAP_SIZE; ++x) {
            for (int y = 0; y < MAP_SIZE; ++y) {
                player_map[x][y].setTerrain(W);
            }
        }

        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                player_map[x][y].setTerrain(map[x][y]);
            }
        }

        int64_t actor_id = 1;
        for (int64_t player_id = 0; player_id < 2; ++player_id) {
            player_states[player_id].bots.clear();
            player_states[player_id].enemy_bots.clear();

            for (int64_t bot_index = 0; bot_index < 10; ++bot_index) {
                player_states[player_id].bots.push_back(
                    player_state::Bot(actor_id++));
            }

            player_states[player_id].towers.clear();
            player_states[player_id].enemy_towers.clear();
            for (int64_t tower_index = 0; tower_index < 10; ++tower_index) {
                player_states[player_id].towers.push_back(
                    player_state::Tower(actor_id++));
            }

            player_states[player_id].map = player_map;
        }
    }
};

TEST_F(PlayerStateTest, GetActorCountsTest) {
    auto actor_counts = getActorCounts(player_states[0]);
    EXPECT_EQ(actor_counts[0][0], 20);

    player_states[0].bots[0].position = DoubleVec2D(3.5, 4.5);
    player_states[0].towers[0].position = DoubleVec2D(2.5, 3.5);

    actor_counts = getActorCounts(player_states[0]);
    EXPECT_EQ(actor_counts[0][0], 18);
    EXPECT_EQ(actor_counts[3][4], 1);
    EXPECT_EQ(actor_counts[2][3], 1);
}

TEST_F(PlayerStateTest, GetBotByIdTest) {
    // Changing the bot properties and checking if the bots are equal
    auto &bot1 = player_states[0].bots[4];
    bot1.id = 2000;
    bot1.hp = 25;

    auto &bot2 = player_states[1].bots[2];
    bot2.id = 65;
    bot2.state = BotState::BLAST;

    EXPECT_EQ(bot1, getBotById(player_states[0], 2000));
    EXPECT_EQ(bot2, getBotById(player_states[1], 65));
    EXPECT_EQ(Bot::null, getBotById(player_states[0], -50));
}

TEST_F(PlayerStateTest, GetTowerByIdTest) {
    // Chanding tower properties and
    auto &tower1 = player_states[0].towers[5];
    tower1.id = 64;
    tower1.hp = 300;

    auto &tower2 = player_states[1].towers[2];
    tower2.id = 4040;
    tower2.state = TowerState::DEAD;

    EXPECT_EQ(tower1, getTowerById(player_states[0], 64));
    EXPECT_EQ(tower2, getTowerById(player_states[1], 4040));
    EXPECT_EQ(Tower::null, getTowerById(player_states[0], 95));
}

TEST_F(PlayerStateTest, FindNearestFlagTest) {
    // Finding the nearest flags
    DoubleVec2D pos1(0.5, 0.5);
    DoubleVec2D pos2(2.5, 2.5);
    DoubleVec2D pos3(3.5, 4.5);
    DoubleVec2D nearest_point1 =
        findNearestFlagPosition(player_states[0], pos1);
    DoubleVec2D nearest_point2 =
        findNearestFlagPosition(player_states[0], pos2);
    DoubleVec2D nearest_point3 =
        findNearestFlagPosition(player_states[0], pos3);

    EXPECT_EQ(nearest_point1, DoubleVec2D(2.5, 1.5));
    EXPECT_EQ(nearest_point2, DoubleVec2D(2.5, 2.5));
    EXPECT_EQ(nearest_point3, DoubleVec2D(2.5, 3.5));
}

TEST_F(PlayerStateTest, FindNearestFreePositionTest) {
    // Finding the nearest buildable offsets
    DoubleVec2D pos1(0.2, 0.8);
    DoubleVec2D pos2(1.5, 1.5);
    DoubleVec2D pos3(3.5, 4.5);
    DoubleVec2D nearest_point1 =
        findNearestFreePosition(player_states[0], pos1);
    DoubleVec2D nearest_point2 =
        findNearestFreePosition(player_states[0], pos2);
    DoubleVec2D nearest_point3 =
        findNearestFreePosition(player_states[0], pos3);

    EXPECT_EQ(nearest_point1, DoubleVec2D(1.5, 0.5));
    EXPECT_EQ(nearest_point2, DoubleVec2D(0.5, 1.5));
    EXPECT_EQ(nearest_point3, DoubleVec2D(3.5, 4.5));
}
