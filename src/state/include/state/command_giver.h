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
    ICommandTaker *state;

    /**
     * Instance of logger to log whenever the user makes invalid state
     * transitions
     */
    logger::ILogger *logger;

    /**
     * Helper function to validate blast request by a bot and make
     * command_taker call blastBot internally
     */
    void blastBot(ActorId actor_id, DoubleVec2D position);

    /**
     * Helper function to validate blast request by a bot and make
     * command_taker call blastTower internally
     *
     * @param actor_id
     */
    void blastTower(ActorId actor_id);

    /**
     * Helper function to validate transform request and make command_taker call
     * TransformBot internally
     */
    void transformBot(ActorId bot_id, DoubleVec2D position);

    /**
     * Helper function to make command_taker call MoveBot internally
     */
    void moveBot(ActorId bot_id, DoubleVec2D position);

    /**
     * Helper function to flip a bot position
     *
     * @param map Refernce to map
     * @param position Position to be flipped
     * @return DoubleVec2D Flipped position
     */
    static DoubleVec2D flipBotPosition(const Map &map, DoubleVec2D position);

    /**
     * Helper function to flip a tower position
     *
     * @param map Reference to a map
     * @param position Position to be flipped
     * @return Vec2D Flipped position
     */
    static DoubleVec2D flipTowerPosition(const Map &map, DoubleVec2D position);

    /**
     * Helper function to check if given bot position is within the map
     *
     * @param map Reference to the map
     * @param position Position which is checked against map constraints
     * @return true    Position is a valid position inside the map
     * @return false   Position is not a valid position in the map
     */
    static bool isValidBotPosition(const Map &map, DoubleVec2D position);

    /**
     * Returns the corresponding offset given a position
     *
     * @param map Reference to map
     * @param position Position
     * @return Vec2D Offset
     */
    static Vec2D getOffset(const Map &map, DoubleVec2D position,
                           PlayerId player_id);

    /**
     * Helper function to check if given tower position is within the map
     *
     * @param map Referece to the map
     * @param position Position which is checked against map constraints
     * @return true    Position is a valid position inside the map
     * @return false   Position is not a valid position in the map
     */
    static bool isValidTowerPosition(const Map &map, DoubleVec2D position,
                                     PlayerId player_id);

    /**
     * Helper function to check if the player has changed the bot state or not
     *
     * @param state_name  The bot's state according to the main state
     * @param player_state_name  The bot's state according to the player state
     * @return true  Player has changed the bot state
     * @return false Player has not changed the bot state
     */
    static bool hasBotStateChanged(BotStateName state_name,
                                   player_state::BotState player_state_name);

    /**
     * Helper function to check if the player has changed the tower state or not
     *
     * @param state_name The towers's state according to the main state
     * @param player_state_name The tower's state according to the player state
     * @return true Player has changed the tower state
     * @return false Player has not changed the bot state
     */
    static bool
    hasTowerStateChanged(TowerStateName state_name,
                         player_state::TowerState player_state_name);

    /**
     * Helper function to check if given transform position is a spawn position
     *
     * @param map Reference to map
     * @param position Position which is check against spawn position
     * @return true Position is a spawn position
     * @return false Position is not a spawn position
     */
    static bool isSpawnOffset(const Map &map, DoubleVec2D position,
                              PlayerId player_id);

    /**
     * Given a double position, make it not dangerous i.e., round it to 6
     * decimals
     * @param value
     * @return sanitized double
     */
    static DoubleVec2D sanitize(DoubleVec2D position);

  public:
    CommandGiver();

    CommandGiver(ICommandTaker *state, logger::ILogger *logger);

    /**
     * @see ICommandGiver#runCommands
     */
    void runCommands(std::array<player_state::State, 2> &player_states,
                     std::array<bool, 2> skip_turns) override;
};

} // namespace state
