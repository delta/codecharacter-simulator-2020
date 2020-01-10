/**
 * @file map.h
 * Declares the Map class that specifies the game map
 */

#pragma once

#include "state/utilities.h"

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
  size_t GetSize() const;

  /**
   * Get the Terrain type of the element from any position on the map
   *
   * @param x
   * @param y
   * @return the tile's terrain type
   */
  TerrainType GetTerrainType(int64_t x, int64_t y) const;

  TerrainType GetTerrainType(double x, double y) const;

  /**
   * Set the Terrain type of the element from any position on the map
   *
   * @param x
   * @param y
   * @param terrainType the tile's terrain type
   */
  void SetTerrainType(int64_t x, int64_t y, TerrainType terrainType);
};

} // namespace state
