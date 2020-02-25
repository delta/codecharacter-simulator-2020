/**
 * @file path_planner.h
 * Declares the PathPlanner class
 */

#pragma once

#include "state/map/map.h"
#include "state/path_planner/interfaces/i_path_planner.h"
#include "state/path_planner/path_graph.h"

namespace state {

class PathPlanner : public IPathPlanner {

    /**
     * Map instance to determine offset terrain
     * Not changed during game, acts as a reference for the original terrain of
     * a given offset
     */
    std::unique_ptr<Map> map;

    /**
     * PathGraph class handles all path calculations
     * PathGraph also keeps track of towers as obstacles
     */
    PathGraph path_graph;

    /**
     * Helper function to get a point along the direction of a line segment
     * given the endpoints at a specific distance
     * @param point_a Start point of line segment
     * @param point_b End point of line segment
     * @param distance Target distance from start point
     * @return DoubleVec2D Target point
     */
    static DoubleVec2D getPointAlongLine(const DoubleVec2D &point_a,
                                         const DoubleVec2D &point_b,
                                         const double_t &distance);

    /**
     * Helper function for path planner to check if a position is inside the map
     * boundaries for a given player
     * NOTE: For tower, the boundaries with one of the coordinates as MAP_SIZE
     * is invalid
     * @param position Position to be checked
     * @param player_id
     * @param is_tower True, if the check if for tower operations
     * @return True, if the position is valid for the player
     *         False, otherwise
     */
    bool isInMapRange(DoubleVec2D position, PlayerId player_id,
                      ActorType actor_type);

    /**
     * Given a position, return the offset
     * @param position
     * @param player_id
     * @return position of base
     */
    Vec2D getOffset(const DoubleVec2D &position, const PlayerId &player_id);

    /**
     * Given a position, get all offsets which the position intersects with
     * @param position
     * @return Vector of offsets intersecting the given position
     */
    std::vector<Vec2D> getAdjoiningOffsets(DoubleVec2D position);

  public:
    PathPlanner(std::unique_ptr<Map> p_map);

    /**
     * Check if a given offset is blocked and cannot be traversed
     * @param position
     * @return True, if offset is blocked. False, otherwise.
     */
    bool isOffsetBlocked(const Vec2D &position) const;

    /**
     * Check if it is possible to construct a tower at position by a player
     * @param position
     * @param player_id Player who builds the tower
     * @return True, if a tower can be constructed at given position.
     *         False, otherwise
     */
    bool isValidTowerPosition(DoubleVec2D position, PlayerId player_id);

    /**
     * Returns the terrain type of the position
     *
     * @param position Position for which terrain type is requested
     * @return TerrainType Terrain type of position
     */
    TerrainType getTerrainType(DoubleVec2D position);

    /**
     * Check if this position is a valid position where a bot can exist
     * @param position
     * @return True, if a bot can exist at this position
     *         False, if not
     */
    bool isValidBotPosition(DoubleVec2D position);

    /**
     * Returns the terrain type of the position
     *
     * @param position Position for which terrain type is requested
     * @return TerrainType Terrain type of the position
     */
    TerrainType getTerrainType(DoubleVec2D position) const;

    /**
     * Build a new tower at given position
     * @param position
     * @return Vec2D If successfully built, Offset in which tower has been built
     *               Null, otherwise
     */
    Vec2D buildTower(DoubleVec2D position, PlayerId player_id);

    /**
     * Destroy a tower at given position
     * @param tower_offset
     */
    bool destroyTower(Vec2D tower_offset);

    /**
     * Called every turn to update the path graph based on current obstacles
     */
    void recomputePathGraph();

    /**
     * @see IPathPlanner#GetNextPosition
     */
    DoubleVec2D getNextPosition(DoubleVec2D source, DoubleVec2D destination,
                                size_t speed) override;
};

} // namespace state
