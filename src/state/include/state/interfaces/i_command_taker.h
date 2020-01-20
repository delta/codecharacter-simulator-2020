/**
 * @file i_command_taker.h
 * Interface for game state changing calls
 */

#pragma once

#include "physics/vector.hpp"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/state_export.h"
#include "state/utilities.h"

#include <memory>

namespace state {

/**
 * ICommandTaker interface to define the game action methods
 */
class STATE_EXPORT ICommandTaker : public IUpdatable {
  public:
    virtual ~ICommandTaker() {}

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
     *
     * @throw      std::exception  if the operation was not possible
     */
    virtual void transformBot(PlayerId player_id, ActorId bot_id) = 0;

    /**
     * Blast a bot or a tower and damage units nearby
     *
     * @param[in]  player_id     Player to act upon
     * @param[in]  actor_id      Actor id to act upon
     *
     * @throw      std::exception  if the operation was not possible
     */
    virtual void blastActor(PlayerId player_id, ActorId actor_id) = 0;

    /**
     * Get map from state
     *
     * @return map
     */
    virtual const std::unique_ptr<Map> getMap() const = 0;

    /**
     * Get game scores from state
     *
     * @return scores
     */
    virtual const std::array<int64_t, 2> getScores(bool game_over) const = 0;

    /**
     * Check if the game is over
     *
     * @param[out] winner If the game is over, who is the winner
     *             PLAYER1 if Player 1 wins
     *             PLAYER2 if Player 2 wins
     *             PLAYER_NULL if it's a draw (both teams had an equal
     * score)
     *
     * @return true If the game is over
     * @return false If the game is not over
     */
    virtual bool isGameOver(PlayerId &winner) = 0;
};
} // namespace state
