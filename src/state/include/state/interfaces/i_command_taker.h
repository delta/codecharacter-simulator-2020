/**
 * @file i_command_taker.h
 * Interface for game state changing calls
 */

#pragma once

#include "physics/vector.hpp"
#include "state/interfaces/i_updatable.h"
#include "state/state_export.h"
#include "state/utilities.h"

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
  virtual void MoveSoldier(PlayerId player_id, int64_t bot_id,
                           physics::Vector<int64_t> position) = 0;

  /**
   * Handles bot transform
   *
   * @param[in]  player_id     Player to act upon
   * @param[in]  bot_id    Bot to act upon
   *
   * @throw      std::exception  if the operation was not possible
   */
  virtual void TransformBot(PlayerId player_id, int64_t bot_id) = 0;

  /**
   * Blast a bot and destroy units nearby
   *
   * @param[in]  player_id     Player to act upon
   * @param[in]  bot_id      Bot to act upon
   *
   * @throw      std::exception  if the operation was not possible
   */
  virtual void BlastBot(PlayerId player_id, int64_t bot_id) = 0;
};
} // namespace state
