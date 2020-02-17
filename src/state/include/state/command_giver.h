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
    void transformBot(PlayerId player_id, ActorId bot_id, DoubleVec2D position);

    /**
     * Helper function to make command_taker call MoveBot internally
     */
    void moveBot(PlayerId player_id, ActorId bot_id, DoubleVec2D position);

    /**
     * Helper function to flip a position for enemy units
     *
     * @param position Position that gets flipped
     * @return DoubleVec2D Flipped position
     */
    DoubleVec2D flipBotPosition(DoubleVec2D pos) const;

    /**
     * Helper function to flip a position for enemy units
     *
     * @param position Tower position to be flipped
     * @return Vec2D Flipped tower position
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

    /**
     * Helper function to check if given grid position is within the map
     *
     * @param position Position to be checked
     * @return true Valid position
     * @return false Invalid position
     */
    bool isValidTowerPosition(Vec2D position) const;

    /**
     * Helper function to check if the player has changed the bot state or not
     *
     * @param state_name  The bot's state according to the main state
     * @param player_state_name  The bot's state according to the player state
     * @return true  Player has changed the bot state
     * @return false Player has not changed the bot state
     */
    bool hasBotStateChanged(BotStateName state_name,
                            player_state::BotState player_state_name) const;

    /**
     * Helper function to check if the player has changed the tower state or not
     *
     * @param state_name The towers's state according to the main state
     * @param player_state_name The tower's state according to the player state
     * @return true Player has changed the tower state
     * @return false Player has not changed the bot state
     */
    bool hasTowerStateChanged(TowerStateName state_name,
                              player_state::TowerState player_state_name) const;

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
