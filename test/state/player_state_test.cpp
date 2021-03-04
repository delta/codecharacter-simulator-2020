#include "state/player_state.h"
#include "gtest/gtest.h"
#include <sstream>

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
        for (size_t x = 0; x < MAP_SIZE; ++x) {
            for (size_t y = 0; y < MAP_SIZE; ++y) {
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

TEST_F(PlayerStateTest, FindTowerByPosition) {
    // Adding towers to the player state
    auto tower1 = Tower(6);
    tower1.position = DoubleVec2D(2.5, 2.5);
    auto tower2 = Tower(7);
    tower2.position = DoubleVec2D(3.5, 3.5);

    player_states[0].towers.push_back(tower1);
    player_states[0].enemy_towers.push_back(tower2);

    auto tower = getTowerByPosition(player_states[0], DoubleVec2D(2.1, 2.2));
    EXPECT_EQ(tower, tower1);

    tower = getTowerByPosition(player_states[0], DoubleVec2D(3.5, 3.5));
    EXPECT_EQ(tower, tower2);

    tower = getTowerByPosition(player_states[0], DoubleVec2D(4.1, 4.2));
    EXPECT_EQ(tower, Tower::null);
}

TEST_F(PlayerStateTest, GetOffsetFromPosition) {
    auto offset1 = getOffsetFromPosition(DoubleVec2D(2.5, 2.9));
    auto offset2 = getOffsetFromPosition(DoubleVec2D(2, 2));
    EXPECT_EQ(offset1, Vec2D(2, 2));
    EXPECT_EQ(offset2, Vec2D(2, 2));
}

TEST_F(PlayerStateTest, TowerTest) {
    Tower &tower1 = player_states[0].towers[2];
    tower1.id = 10;
    Tower &tower2 = player_states[1].towers[3];
    tower2.id = 19;
    Tower tower3(tower1);
    Tower &tower4 = player_states[0].towers[5];
    tower4.id = 29;

    EXPECT_EQ(tower1.id, 10);
    EXPECT_EQ(tower2.id, 19);
    EXPECT_EQ(tower3.id, 10);
    EXPECT_EQ(tower4.id, 29);

    EXPECT_NE(tower1, tower2);
    EXPECT_EQ(tower1, tower3);

    // Initial values
    EXPECT_EQ(tower4.position, DoubleVec2D(0, 0));
    EXPECT_EQ(tower4.state, player_state::TowerState::IDLE);
    EXPECT_EQ(tower4.age, 0);
    EXPECT_EQ(tower4.hp, 100);
    EXPECT_EQ(tower4.impact_radius, 0);
    EXPECT_EQ(tower4.blasting, false);

    tower1.blast();
    EXPECT_EQ(tower1.blasting, true);
}

TEST_F(PlayerStateTest, BotTest) {
    // different methods of construction
    Bot &bot1 = player_states[0].bots[2];
    bot1.id = 11;
    Bot &bot2 = player_states[1].bots[2];
    bot2.id = 29;
    bot2.position = {2, 2};
    player_state::Bot bot3(bot2);

    EXPECT_EQ(bot1.id, 11);
    EXPECT_EQ(bot1.final_destination, DoubleVec2D::null);
    EXPECT_EQ(bot1.transform_destination, DoubleVec2D::null);
    EXPECT_EQ(bot1.transforming, false);
    EXPECT_EQ(bot1.blasting, false);
    EXPECT_EQ(bot1.destination, DoubleVec2D::null);
    EXPECT_EQ(bot1.position, DoubleVec2D(0, 0));

    // == & != operator checks
    EXPECT_EQ(bot2, bot3);
    EXPECT_NE(bot1, bot2);

    // target_position!=position case
    bot1.blast({10, 10});
    EXPECT_EQ(bot1.final_destination, DoubleVec2D(10, 10));
    EXPECT_EQ(bot1.blasting, false);

    bot1.reset();
    EXPECT_EQ(bot1.transform_destination, DoubleVec2D::null);
    EXPECT_EQ(bot1.final_destination, DoubleVec2D::null);
    EXPECT_EQ(bot1.transforming, false);
    EXPECT_EQ(bot1.destination, DoubleVec2D::null);
    EXPECT_EQ(bot1.blasting, false);

    // transforms test on bot1
    bot1.position = {9, 9};
    bot1.transform({9, 9});
    EXPECT_EQ(bot1.transforming, true);

    // target_position==postion case on bot2
    EXPECT_EQ(bot2.position, DoubleVec2D(2, 2));
    bot2.blast({2, 2});
    EXPECT_EQ(bot2.transforming, false);
    EXPECT_EQ(bot2.transform_destination, DoubleVec2D::null);
    EXPECT_EQ(bot2.final_destination, DoubleVec2D::null);
    EXPECT_EQ(bot2.position, DoubleVec2D(2, 2));
    EXPECT_EQ(bot2.blasting, true);

    bot2.reset();
    EXPECT_EQ(bot2.transform_destination, DoubleVec2D::null);
    EXPECT_EQ(bot2.final_destination, DoubleVec2D::null);
    EXPECT_EQ(bot2.transforming, false);
    EXPECT_EQ(bot2.destination, DoubleVec2D::null);
    EXPECT_EQ(bot2.blasting, false);

    bot2.transform({10, 10});
    EXPECT_EQ(bot2.transform_destination, DoubleVec2D(10, 10));
    EXPECT_EQ(bot2.transforming, false);

    bot2.transform();
    EXPECT_EQ(bot2.transforming, true);
}

TEST_F(PlayerStateTest, MapElementTest) {
    auto &elem1 = player_map[1][2];
    elem1.setTerrain(player_state::TerrainType::LAND);
    EXPECT_EQ(elem1.getTerrain(), player_state::TerrainType::LAND);
}

TEST_F(PlayerStateTest, OverloadOperatorsTest) {
    std::ostringstream result;
    std::ostringstream expected;

    // Tower and TowerState ostream overloads test
    auto &tower = player_states[0].towers[1];
    tower.id = 10;
    tower.state = TowerState::DEAD;
    // towerstate ostream << test
    result << tower.state;
    EXPECT_EQ(result.str(), string("DEAD"));
    result.str(""); // clears the stream
    result.clear();
    // << for Tower class
    expected << "Tower(id: " << tower.id << ") {" << endl;
    expected << "   hp: " << tower.hp << endl;
    expected << "   state: " << tower.state << endl;
    expected << "}" << endl;
    result << tower;
    EXPECT_EQ(result.str(), expected.str());
    result.str("");
    result.clear();
    expected.str("");
    expected.clear();

    // Bot and BotState ostream overload tests
    auto &bot = player_states[1].bots[2];
    bot.id = 29;
    bot.state = BotState::BLAST;
    // BotState <<
    result << bot.state;
    EXPECT_EQ(result.str(), string("BLAST"));
    result.str(""); // clears the stream
    result.clear();
    // << for Bot
    expected << "Bot(id: " << bot.id << ") {" << endl;
    expected << "   hp: " << bot.hp << endl;
    expected << "   position: " << bot.position << endl;
    expected << "   state: " << bot.state << endl;
    expected << "}" << endl;
    result << bot;
    EXPECT_EQ(expected.str(), result.str());
}
