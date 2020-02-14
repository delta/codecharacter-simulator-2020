/**
 * @file command_giver.h
 * Validates user's actions and gives command taker the necessary valid
 * instructions
 */

#pragma once

#include "logger/interfaces/i_logger.h"
#include "state/interfaces/i_command_giver.h"
#include "state/interfaces/i_command_taker.h"
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
    void blastActor(PlayerId player_id, ActorId actor_id, DoubleVec2D position);

    /**
     * Helper function to validate transform request and make command_taker call
     * TransformBot internally
     */
    void transformBot(PlayerId player_id, ActorId unit_id,
                      DoubleVec2D position);

    /**
     * Helper function to use make command_taker call MoveBot internally
     */
    void moveBot(PlayerId player_id, ActorId bot_id, DoubleVec2D position);

    /**
     * Helper function to flip a position for enemy units
     *
     * @param position Position that gets flipped
     * @return DoubleVec2D Flipped position
     */
    DoubleVec2D flipBotPosition(DoubleVec2D pos);

    /**
     * Helper function to flip a position for enemy units
     */
    Vec2D flipTowerPosition(Vec2D position) const;

    /**
     * Helper function to check if given position is within the map
     *
     * @param position Position which is checked against map constraints
     * @return true    Position is a valid position inside the map
     * @return false   Position is not a valid position in the map
     */
    bool isValidBotPosition(DoubleVec2D position) const;

  public:
    /**
     * Helper function to check if given grid position is within the map
     */
    bool isValidTowerPosition(Vec2D position) const;

    /**
     * Helper function to get corresponding tower position from the bot position
     */
    Vec2D getTowerPositionFromBotPosition(DoubleVec2D position) const;

  public:
    /**
     * Helper function to get the map size
     */
    size_t getMapSize() const;

  public:
    CommandGiver();

    CommandGiver(std::unique_ptr<ICommandTaker> state,
                 std::unique_ptr<logger::ILogger> logger);

    /**
     * @see ICommandGiver#runCommands
     */
    void runCommands(std::array<player_state::State, 2> &player_states,
                     std::array<bool, 2> skip_turns) override;
};

} // namespace state
