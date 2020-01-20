/**
 * @file i_state_syncer.h
 * Responsible to coordinate between command giver and command taker
 */

#pragma once

#include "state/interfaces/i_command_giver.h"
#include "state/interfaces/i_command_taker.h"
#include "state/player_state.h"
#include "state/utilities.h"

#include <array>

namespace state {

class STATE_EXPORT IStateSyncer {
  public:
    virtual ~IStateSyncer(){};

    /**
     * Method to update the main state
     * @param [in] player_states Reference to the two player states
     * @param [in] skip_turn True if player's turns shouldn't be executed
     */
    virtual void
    updateMainState(std::array<player_state::State, 2> &player_states,
                    std::array<bool, 2> skip_turns) = 0;

    /**
     * Method to update the two player state instances with the new values from
     * the updated main state.
     *
     * @param[inout] player_states Reference to two player states
     */
    virtual void
    updatePlayerStates(std::array<player_state::State, 2> &player_states) = 0;

    /**
     * Check if the game is over
     *
     * @param[out] winner If the game is over, who is the winner
     *             PLAYER1 if Player 1 wins
     *             PLAYER2 if Player 2 wins
     *             PLAYER_NULL if it's a draw (both teams die simultaneously)
     *
     * @return true If the game is over
     * @return false If the game is not over
     */
    virtual bool isGameOver(PlayerId &winner) = 0;

    /**
     * Method to get both players' scores
     *
     * @param game_over Signal that the game is over
     */
    virtual std::array<int64_t, 2> getScores(bool game_over) = 0;
};

} // namespace state
