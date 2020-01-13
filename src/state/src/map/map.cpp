/**
 * @file map.cpp
 * Defines the Map class
 */

#include "state/map/map.h"
#include <utility>

namespace state {

Map::Map(std::vector<std::vector<TerrainType>> map, size_t map_size)
    : map(std::move(map)), map_size(map_size) {}

size_t Map::GetSize() const { return map_size; }

TerrainType Map::GetTerrainType(int64_t x, int64_t y) const {
    return map[x][y];
}

TerrainType Map::GetTerrainType(double x, double y) const {
    return map[(int64_t)x][(int64_t)y];
}

void Map::SetTerrainType(int64_t x, int64_t y, TerrainType terrainType) {
    map[x][y] = terrainType;
}

} // namespace state
