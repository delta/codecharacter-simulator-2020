/**
 * @file i_command_giver.h
 * Interface for command giver which validates and run's user's commands
 */

#pragma once

#include "state/player_state.h"
#include <array>

namespace state {
class STATE_EXPORT ICommandGiver {
  public:
    /*
    **  Initializes the command giver interface
    */
    virtual ~ICommandGiver();

    /**
     * Runs the necessary commands on the command taker (state)
     *
     * @param[in] player_states Player state from which we get commands to run
     * @param[in] skip_player_turn If true for a player, turn is not processed
     */
    virtual void RunCommands(std::array<player_state::State, 2> player_states,
                             std::array<bool, 2> skip_turns) = 0;
};

} // namespace state
