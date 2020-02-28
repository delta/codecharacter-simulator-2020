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

TerrainType Map::getTerrainType(size_t x, size_t y) const { return map[x][y]; }

TerrainType Map::getTerrainType(double_t x, double_t y) const {
    return map[(size_t) x][(size_t) y];
}

TerrainType Map::getTerrainType(DoubleVec2D position) const {
    return map[std::floor(position.x)][std::floor(position.y)];
}

TerrainType Map::getTerrainType(Vec2D position) const {
    return map[position.x][position.y];
}

void Map::setTerrainType(size_t x, size_t y, TerrainType terrainType) {
    map[x][y] = terrainType;
}

} // namespace state
