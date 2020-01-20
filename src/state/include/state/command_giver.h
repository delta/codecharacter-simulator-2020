/**
 * @file command_giver.h
 * Validates user's actions and gives command taker the necessary valid
 * instructions
 */

#pragma once

#include "interfaces/i_command_giver.h"
#include "interfaces/i_command_taker.h"
#include "logger/interfaces/i_logger.h"
#include "state/player_state.h"
#include "state/state.h"
#include "state/utilities.h"
#include <memory>

namespace state {
class STATE_EXPORT CommandGiver : public ICommandGiver {
  private:
    /**
     * Instance of overall state that player can't modify
     */
    std::unique_ptr<ICommandTaker> state;

    /**
     * Instance of logger to log whenever the user makes invalid state
     * transitions
     */
    std::unique_ptr<logger::ILogger> logger;

    /**
     * Helper function to validate blast request by an actor and make
     * command_taker call
     * BlastBot internally
     */
    void BlastActor(PlayerId player_id, ActorId actor_id);

    /**
     * Helper function to validate transform request and make command_taker call
     * TransformBot internally
     */
    void TransformBot(PlayerId player_id, ActorId unit_id);

    /**
     * Helper function to use make command_taker call BlastBot internally
     */
    void MoveBot(PlayerId player_id, int64_t bot_id,
                 physics::Vector<int64_t> position);

    /**
     * Helper function to flip a position for enemy units
     */
    DoubleVec2D FlipPosition(DoubleVec2D pos);

    /**
     * Helper function to check if given position is within the map
     */
    bool IsValidPosition(DoubleVec2D pos) const;

    /**
     * Helper function to check whether the offset given is within the map
     */
    bool IsValidOffset(DoubleVec2D pos) const;

  public:
    CommandGiver();

    CommandGiver(std::unique_ptr<ICommandTaker> state,
                 std::unique_ptr<logger::ILogger> logger);

    /**
     * Validates the actions taken by the player and if valid, makes calls to
     * command taker to make state changes
     */
    void RunCommands(std::array<player_state::State, 2> player_states,
                     std::array<bool, 2> skip_turns);
};

} // namespace state
