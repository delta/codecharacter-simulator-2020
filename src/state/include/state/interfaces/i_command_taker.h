/**
 * @file i_command_taker.h
 * Interface for game state changing calls
 */

#pragma once

#include "physics/vector.hpp"
#include "state/actor/bot.h"
#include "state/actor/tower.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/transform_request.h"
#include "state/utilities.h"

#include <memory>

namespace state {

/**
 * ICommandTaker interface to define the game action methods
 */
class STATE_EXPORT ICommandTaker {
  public:
    virtual ~ICommandTaker(){};

    /**
     * Triggers updation of the main state
     */
    virtual void update() = 0;

    /**
     * Handles bot movement
     *
     * @param[in]  player_id     Player to act upon
     * @param[in]  bot_id    Bot to act upon
     * @param[in]  position      New position to move the bot to
     *
     * @throw      std::exception  if the operation was not possible
     */
    virtual void moveBot(ActorId bot_id, DoubleVec2D position) = 0;

    /**
     * Handles bot transform
     *
     * @param[in]  player_id     Player to act upon
     * @param[in]  bot_id    Bot to act upon
     * @param[in] position Position on which the bot should transform into a
     * tower
     *
     * @throw      std::exception  if the operation was not possible
     */
    virtual void transformBot(PlayerId player_id, ActorId bot_id,
                              DoubleVec2D position) = 0;

    /**
     * Blasts a bot and destroy units nearby
     *
     * @param[in]  actor_id      Actor id to act upon
     * @param[in] position Position where the actor should blast
     *
     * @throw      std::exception  if the operation was not possible
     */
    virtual void blast(ActorId actor_id, DoubleVec2D position) = 0;

    /**
     * Blasts a tower and destroy units nearby
     *
     * @param actor_id
     */
    virtual void blast(ActorId actor_id) = 0;

    /**
     * Get map from state
     *
     * @return Map*
     */
    virtual Map *getMap() const = 0;

    /**
     * Gets the Score Manager
     *
     * @return ScoreManager*
     */
    virtual ScoreManager *getScoreManager() const = 0;

    /**
     * Get the Path Planner
     *
     * @return PathPlanner*
     */
    virtual PathPlanner *getPathPlanner() const = 0;

    /**
     * Get game scores from state
     *
     * @return scores
     */
    virtual std::array<uint64_t, 2> getScores() const = 0;

    /**
     * Returns all the bots in the state
     *
     * @return bots
     */
    virtual std::array<std::vector<Bot *>, 2> getBots() = 0;

    /**
     * Returns all the towers in the state
     *
     * @return towers
     */
    virtual std::array<std::vector<Tower *>, 2> getTowers() = 0;

    /**
     * Get the Transform Requests object
     *
     * @return std::array<std::vector<TransformRequest *>, 2>
     */
    virtual std::array<std::vector<TransformRequest *>, 2>
    getTransformRequests() = 0;

    /**
     * Removes all the dead actors from state
     */
    virtual void removeDeadActors() = 0;
};
} // namespace state
