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
     * command_taker call BlastActor internally
     */
    void blastActor(PlayerId player_id, ActorId actor_id);

    /**
     * Helper function to validate transform request and make command_taker call
     * TransformBot internally
     */
    void transformBot(PlayerId player_id, ActorId unit_id);

    /**
     * Helper function to use make command_taker call BlastBot internally
     */
    void moveBot(PlayerId player_id, ActorId bot_id, DoubleVec2D position);

    /**
     * Helper function to flip a position for enemy units
     *
     * @param position Position that gets flipped
     * @return DoubleVec2D Flipped position
     */
    DoubleVec2D flipPosition(DoubleVec2D position) const;

    /**
     * Helper function to check if given position is within the map
     *
     * @param position Position which is checked against map constraints
     * @return true    Position is a valid position inside the map
     * @return false   Position is not a valid position in the map
     */
    bool isValidPosition(DoubleVec2D position) const;

  public:
    /**
     * Constructors
     */
    CommandGiver();

    CommandGiver(std::unique_ptr<ICommandTaker> state,
                 std::unique_ptr<logger::ILogger> logger);

    /**
     * @see ICommandGiver#runCommands
     */
    void runCommands(std::array<player_state::State, 2> player_states,
                     std::array<bool, 2> skip_turn) override;
};

} // namespace state