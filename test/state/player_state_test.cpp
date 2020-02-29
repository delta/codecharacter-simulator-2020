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
        vector<vector<player_state::TerrainType>> map = {{L, L, L, W, W},
                                                         {L, W, W, W, W},
                                                         {L, F, F, W, W},
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
    }
};

TEST_F(PlayerStateTest, FindNearestFlagTest) {
    // Finding the nearest flags
    Vec2D pos1(0, 0);
    Vec2D pos2(2, 2);
    Vec2D pos3(4, 4);
    Vec2D nearest_point1 = findNearestFlagLocation(player_map, pos1);
    Vec2D nearest_point2 = findNearestFlagLocation(player_map, pos2);
    Vec2D nearest_point3 = findNearestFlagLocation(player_map, pos3);

    EXPECT_EQ(nearest_point1, Vec2D(2, 1));
    EXPECT_EQ(nearest_point2, Vec2D(2, 2));
    EXPECT_EQ(nearest_point3, Vec2D(3, 2));
}

TEST_F(PlayerStateTest, FindNearestOffset) {
    // Finding the nearest buildable offsets
    Vec2D pos1(0, 0);
    Vec2D pos2(1, 4);
    Vec2D pos3(2, 2);
    Vec2D nearest_point1 = findNearestBuildableOffset(player_map, pos1);
    Vec2D nearest_point2 = findNearestBuildableOffset(player_map, pos2);
    Vec2D nearest_point3 = findNearestBuildableOffset(player_map, pos3);

    EXPECT_EQ(nearest_point1, Vec2D(0, 0));
    EXPECT_EQ(nearest_point2, Vec2D(3, 4));
    EXPECT_EQ(nearest_point3, Vec2D(2, 2));
}
