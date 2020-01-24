/**
 * @file path_graph.h
 * Declares a wrapper for a graph for path calculations
 */

#pragma once

#include "state/path_planner/graph/graph.h"

namespace state {

class PathGraph {
  private:
    /**
     * The size of the map
     */
    size_t map_size;

    /**
     * boolean array of (map_size x map_size)
     * true, if the cell can be moved on
     * false, otherwise
     */
    std::vector<std::vector<bool>> valid_terrain;

    /**
     * Graph of waypoints
     */
    Graph graph;

    /**
     * Remove all waypoints and edges
     */
    void resetWaypointGraph();

    /**
     * Check if given position is on valid terrain
     * @param position
     * @return True, if position can be traversed, false otherwise
     */
    bool isValidPosition(const DoubleVec2D &position) const;

    /**
     * Check if given position is on valid terrain
     * @param x
     * @param y
     * @return True, if position can be traversed, false otherwise
     */
    bool isValidPosition(const double_t &x, const double_t &y) const;

    /**
     * Add a new waypoint and create edges
     * @param position
     */
    void addWaypoint(const DoubleVec2D &position);

    /**
     * Remove a waypoint and corresponding edges
     * @param position
     */
    void removeWaypoint(const DoubleVec2D &position);

    /**
     * Return all waypoints constructed
     * @return vector of waypoint positions
     */
    boost::unordered_set<DoubleVec2D> getWaypoints() const;

    /**
     * Recalculate waypoints from valid_terrain graph
     */
    void recomputeWaypoints();

    /**
     * Check if two points are directly reachable from each other
     * @param point_a
     * @param point_b
     * @return bool true, if they are directly reachable
     */
    bool arePointsDirectlyReachable(DoubleVec2D point_a,
                                    DoubleVec2D point_b) const;

    /**
     * Recalculate all edges from a single waypoint
     */
    void recomputeWaypointEdges(const DoubleVec2D &position);

    /**
     * Recalculate all edges between all waypoints
     */
    void recomputeWaypointEdges();

    /**
     * Return all integers in the range and include x_a and x_b
     * Eg: For (2.5, 5.5), result = {2.5, 3, 4, 5, 5.5}
     * @param a
     * @param b
     * @return Vector of result
     */
    std::vector<double_t> generateIntersections(double_t a, double_t b) const;

    /**
     * Get the slope of line between two points
     * @param a
     * @param b
     * @return Slope
     */
    double_t getSlope(const DoubleVec2D &a, const DoubleVec2D &b) const;

    /**
     * Check if a path along same x or same y is traversable
     * @param start
     * @param destination
     * @return True, if traversable. Else, false
     */
    bool isStraightLineTraversable(DoubleVec2D start,
                                   DoubleVec2D destination) const;

  public:
    PathGraph(size_t p_map_size, std::vector<std::vector<bool>> p_valid_terrain,
              Graph p_graph);

    /**
     * Set map size and map terrain
     * @param map_size
     * @param p_valid_terrain
     */
    void setValidTerrain(std::vector<std::vector<bool>> p_valid_terrain);

    /**
     * Recalculate waypoints and edges from valid_terrain
     */
    void recomputeWaypointGraph();

    /**
     * Adds obstacle in a position
     * @param position
     */
    void addObstacle(const DoubleVec2D &position);

    /**
     * Add a list of obstacles
     * @param position
     */
    void addObstacles(const std::vector<DoubleVec2D> &positions);

    /**
     * Remove obstacle from a position
     * @param position
     */
    void removeObstacle(const DoubleVec2D &position);

    /**
     * Get path from one position to another
     * @param start_position
     * @param end_position
     */
    std::vector<DoubleVec2D> getPath(DoubleVec2D start_position,
                                     DoubleVec2D end_position);
};

} // namespace state
