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
     * Map instance to determine terrain
     * Not changed during game, acts as a reference for the original terrain of
     * a given position
     */
    std::unique_ptr<Map> map;

    /**
     * PathGraph class handles all path calculations
     * PathGraph also keeps track of towers as obstacles
     */
    PathGraph path_graph;

    /**
     * Helper function to get a point along the direction of a line segment
     * given the endpoints
     * @param point_a Start point of line segment
     * @param point_b End point of line segment
     * @param distance Target distance from start point
     * @return DoubleVec2D Target point
     */
    static DoubleVec2D getPointAlongLine(DoubleVec2D point_a,
                                         DoubleVec2D point_b,
                                         double_t distance);

  public:
    PathPlanner(std::unique_ptr<Map> p_map);

    /**
     * Check if a given position is blocked and cannot be traversed
     * @param position
     * @return True, if position is movable. False, otherwise.
     */
    bool isPositionBlocked(DoubleVec2D position);

    /**
     * Build a new tower at given position
     * @param position
     */
    void addTower(DoubleVec2D position);

    /**
     * Destroy a tower at given position
     * @param position
     */
    void removeTower(DoubleVec2D position);

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
