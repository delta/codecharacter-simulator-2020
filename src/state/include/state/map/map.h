/**
 * @file map.h
 * Declares the Map class that specifies the game map
 */

#pragma once

#include "physics/vector.hpp"
#include "state/utilities.h"

#include <cmath>
#include <cstddef>
#include <vector>

namespace state {

/**
 * Map class that holds a 2D matrix of terrain elements, with the map data
 */
class Map {
  private:
    /**
     * 2D vector containing terrain types for the map
     */
    std::vector<std::vector<TerrainType>> map;

    /**
     * Map size, the number of map elements per side of the map
     */
    size_t map_size;

  public:
    /**
     * Constructor
     *
     * @param map
     * @param map_size
     */
    Map(std::vector<std::vector<TerrainType>> map, size_t map_size);

    /**
     * Get the size of the map (returns the template param)
     *
     * @return Width of the map in grid elements
     */
    size_t getSize() const;

    /**
     * Get the Terrain type of the element from any position on the map
     *
     * @param x
     * @param y
     * @return the tile's terrain type
     */
    TerrainType getTerrainType(size_t x, size_t y) const;

    TerrainType getTerrainType(double_t x, double_t y) const;

    TerrainType getTerrainType(DoubleVec2D position) const;

    TerrainType getTerrainType(Vec2D position) const;

    /**
     * Set the Terrain type of the element from any position on the map
     *
     * @param x
     * @param y
     * @param terrainType the tile's terrain type
     */
    void setTerrainType(size_t x, size_t y, TerrainType terrainType);
};

} // namespace state
