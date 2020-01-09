#include "state/map/map.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;

const auto MAP_SIZE = size_t{5};
const auto ELEMENT_SIZE = size_t{10};

const auto L = state::TerrainType::LAND;
const auto W = state::TerrainType::WATER;
const auto F = state::TerrainType::FLAG;

class MapTest : public testing::Test {
protected:
  unique_ptr<state::Map> map;
  MapTest() {
    auto map_matrix = vector<vector<state::TerrainType>>{{
        {L, L, W, L, L},
        {L, W, W, W, L},
        {L, L, F, L, L},
        {L, W, W, W, L},
        {L, L, W, L, L},
    }};

    map = make_unique<state::Map>(map_matrix, MAP_SIZE, ELEMENT_SIZE);
  }
};

TEST_F(MapTest, GetSizeTest) { EXPECT_EQ(map->GetSize(), MAP_SIZE); }

TEST_F(MapTest, GetElementSizeTest) {
  EXPECT_EQ(map->GetElementSize(), ELEMENT_SIZE);
}

TEST_F(MapTest, GetElementByOffsetTest) {
  EXPECT_EQ(map->GetTerrainTypeByOffset(0, 0), L);
  EXPECT_EQ(map->GetTerrainTypeByOffset(1, 1), W);
  EXPECT_EQ(map->GetTerrainTypeByOffset(2, 2), F);
  EXPECT_EQ(map->GetTerrainTypeByOffset(4, 4), L);
  EXPECT_EQ(map->GetTerrainTypeByOffset(0, 4), L);
  EXPECT_EQ(map->GetTerrainTypeByOffset(4, 0), L);
}

TEST_F(MapTest, GetElementByPositionTest) {
  EXPECT_EQ(map->GetTerrainTypeByPosition(0, 0), L);
  EXPECT_EQ(map->GetTerrainTypeByPosition(15, 15), W);
  EXPECT_EQ(map->GetTerrainTypeByPosition(24, 24), F);
  EXPECT_EQ(map->GetTerrainTypeByPosition(49, 49), L);
  EXPECT_EQ(map->GetTerrainTypeByPosition(0, 49), L);
  EXPECT_EQ(map->GetTerrainTypeByPosition(49, 0), L);
}
