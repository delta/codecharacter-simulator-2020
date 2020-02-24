/**
 * @file map.cpp
 * Defines the Map class
 */

#include "state/map/map.h"
#include <utility>

namespace state {

Map::Map(std::vector<std::vector<TerrainType>> map, size_t map_size)
    : map(std::move(map)), map_size(map_size) {}

size_t Map::getSize() const { return map_size; }

TerrainType Map::getTerrainType(uint64_t x, uint64_t y) const {
    return map[x][y];
}

TerrainType Map::getTerrainType(double_t x, double_t y) const {
    return map[(uint64_t) x][(uint64_t) y];
}

void Map::setTerrainType(uint64_t x, uint64_t y, TerrainType terrainType) {
    map[x][y] = terrainType;
}

} // namespace state
