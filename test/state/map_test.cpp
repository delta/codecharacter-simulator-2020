#include "state/map/map.h"

#include <gtest/gtest.h>
#include <memory>

using namespace std;

const auto MAP_SIZE = size_t{5};

const auto L = state::TerrainType::LAND;
const auto W = state::TerrainType::WATER;
const auto T = state::TerrainType::TOWER;
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
            {L, L, W, L, T},
        }};

        map = make_unique<state::Map>(map_matrix, MAP_SIZE);
    }
};

TEST_F(MapTest, GetSizeTest) { EXPECT_EQ(map->getSize(), MAP_SIZE); }

TEST_F(MapTest, GetTerrainTypeInt64Test) {
    EXPECT_EQ(map->getTerrainType((size_t) 0, (size_t) 0), L);
    EXPECT_EQ(map->getTerrainType((size_t) 1, (size_t) 1), W);
    EXPECT_EQ(map->getTerrainType((size_t) 2, (size_t) 2), F);
    EXPECT_EQ(map->getTerrainType((size_t) 4, (size_t) 4), T);
    EXPECT_EQ(map->getTerrainType((size_t) 0, (size_t) 4), L);
    EXPECT_EQ(map->getTerrainType((size_t) 4, (size_t) 0), L);
}

TEST_F(MapTest, GetTerrainTypeDoubleTest) {
    EXPECT_EQ(map->getTerrainType(0.5, 0.18), L);
    EXPECT_EQ(map->getTerrainType(1.599, 1.501), W);
    EXPECT_EQ(map->getTerrainType(2.432, 2.411), F);
    EXPECT_EQ(map->getTerrainType(4.997, 4.999), T);
    EXPECT_EQ(map->getTerrainType(0.11, 4.921), L);
    EXPECT_EQ(map->getTerrainType(4.90, 0.0), L);
}

TEST_F(MapTest, SetTerrainTypeTest) {
    map->setTerrainType(0.0, 0.0, T);
    EXPECT_EQ(map->getTerrainType(0.0, 0.0), T);
    map->setTerrainType(0.0, 0.0, L);
}
