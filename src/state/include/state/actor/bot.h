/**
 * @file bot.h
 *  Declares the Bot Class
 */

#pragma once

#include "state/actor/blaster.h"
#include "state/actor/bot_states/bot_state.h"
#include "state/actor/unit.h"
#include "state/score_manager/score_manager.h"
#include "state/path_planner/path_planner.h"

namespace state {

using ConstructTowerCallback = std::function<void(Bot *)>;

/**
 * Declaration of Bot class
 */
class STATE_EXPORT Bot : public Unit, public Blaster {
  private:
    /**
     *  Controls logic for bot's current state
     */
    std::unique_ptr<BotState> state;

    /**
     * Score manager to update scores
     */
    ScoreManager *score_manager;

    /**
     *  Path Planner to perform movement mechanics
     */
    PathPlanner *path_planner;

    /**
     *  The position at which the bot transitions to blast state
     */
    DoubleVec2D final_destination;

    /**
     *  Tracks final_destination
     */
    bool is_final_destination_set;

    /**
     * Callback to implement effect of bot transformation to tower through state
     */
    ConstructTowerCallback construct_tower_callback;

    /**
     *  The destination after which the bot becomes a tower
     */
    DoubleVec2D transform_destination;

    /**
     *  Tracks transform_destination
     */
    bool is_transform_destination_set;

    /**
     *  True if the bot is going to transform
     *  False if otherwise
     */
    bool is_transforming;

  public:
    /**
     * Construct a new Bot object
     *
     * @param id
     * @param player_id
     * @param hp
     * @param max_hp
     * @param position
     * @param speed
     * @param blast_range
     * @param damage_points
     * @param score_manager
     * @param path_planner
     * @param blast_callback
     * @param construct_tower_callback
     */
    Bot(ActorId id, PlayerId player_id, size_t hp, size_t max_hp,
        DoubleVec2D position, size_t speed, size_t blast_range,
        size_t damage_points, ScoreManager *score_manager,
        PathPlanner *path_planner, BlastCallback blast_callback,
        ConstructTowerCallback construct_tower_callback);

    /**
     * Construct a new Bot object
     *
     * @param player_id
     * @param hp
     * @param max_hp
     * @param position
     * @param speed
     * @param blast_range
     * @param damage_points
     * @param score_manager
     * @param path_planner
     * @param blast_callback
     * @param construct_tower_callback
     */
    Bot(PlayerId player_id, size_t hp, size_t max_hp, DoubleVec2D position,
        size_t speed, size_t blast_range, size_t damage_points,
        ScoreManager *score_manager, PathPlanner *path_planner,
        BlastCallback blast_callback,
        ConstructTowerCallback construct_tower_callback);

    /**
     * Get Bot's Path Planner pointer
     *
     * @return PathPlanner*
     */
    PathPlanner *getPathPlanner() const;

    /**
     *  check if final_destination is set
     *
     * @return true
     * @return false
     */
    bool isFinalDestinationSet() const;

    /**
     *  Sets final_destination with passed destination
     *
     * @param final_destination
     */
    void setFinalDestination(DoubleVec2D p_final_destination);

    /**
     *  Get the value of final_destination
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getFinalDestination() const;

    /**
     *  resets final_destination to DoubleVec2D::null
     */
    void clearFinalDestination();

    /**
     *  check if transform_destination is set
     *
     * @return true
     * @return false
     */
    bool isTransformDestinationSet() const;

    /**
     *  Set the transform_destination property
     *
     * @param transform_destination
     */
    void setTransformDestination(DoubleVec2D p_transform_destination);

    /**
     *  checks if is_transforming property is set or not
     *
     * @return true
     * @return false
     */
    bool isTransforming() const;

    /**
     *  Set the is_transforming property
     *
     * @param p_transforming
     */
    void setTransforming(bool p_transforming);

    /**
     *  Get the transform_destination property
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getTransformDestination() const;

    /**
     *  resets transform_destination
     */
    void clearTransformDestination();

    /**
     *  Method to execute construct_tower_callback
     *
     * @param player_id
     * @param position
     * @param current_hp
     */
    void constructTower();

    /**
     *  Get the current state of bot
     *
     * @return BotStateName
     */
    BotStateName getState() const;

    /**
     * @see Blaster#Blast
     */
    void blast() override;

    /**
     *  Changes bot state to Transform
     */
    void transform();

    /**
     * @see IUpdatable#update
     */
    void update() override;

    /**
     * @see IUpdatable#lateUpdate
     */
    void lateUpdate() override;
};

} // namespace state
