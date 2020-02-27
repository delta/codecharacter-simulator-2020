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
#include "state/utilities.h"

#include <memory>

namespace state {

/**
 * ICommandTaker interface to define the game action methods
 */
class STATE_EXPORT ICommandTaker : public IUpdatable {
  public:
    virtual ~ICommandTaker(){};

    /**
     * Handles bot movement
     *
     * @param[in]  player_id     Player to act upon
     * @param[in]  bot_id    Bot to act upon
     * @param[in]  position      New position to move the bot to
     *
     * @throw      std::exception  if the operation was not possible
     */
    virtual void moveBot(PlayerId player_id, ActorId bot_id,
                         DoubleVec2D position) = 0;

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
     * @param[in]  player_id     Player to act upon
     * @param[in]  actor_id      Actor id to act upon
     * @param[in] position Position where the actor should blast
     *
     * @throw      std::exception  if the operation was not possible
     */
    virtual void blastBot(PlayerId player_id, ActorId actor_id,
                          DoubleVec2D position) = 0;

    /**
     * Blasts a tower and destroy units nearby
     *
     * @param player_id
     * @param actor_id
     */
    virtual void blastTower(PlayerId player_id, ActorId actor_id) = 0;

    /**
     * Get map from state
     *
     * @return map
     */
    virtual Map *getMap() const = 0;

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
     * Removes all the dead actors from state
     */
    virtual void removeDeadActors() = 0;
};
} // namespace state
