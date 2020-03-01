/**
 * @file state_syncer.h
 * Declares the StateSyncer class
 */

#pragma once

#include "logger/interfaces/i_logger.h"
#include "state/interfaces/i_command_giver.h"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_state_syncer.h"
#include <memory>

namespace state {

class STATE_EXPORT StateSyncer : public IStateSyncer {
  private:
    /**
     * Instance of the state for giving commands to
     */
    std::unique_ptr<ICommandGiver> command_giver;

    /**
     * Instance of command giver for giving commands to state
     */
    std::unique_ptr<ICommandTaker> state;

    /**
     * Instance of logger to log every turn and to log errors
     */
    logger::ILogger *logger;

    /**
     * Flips a given bots position
     * (Internally calls flip tower position)
     *
     * @param map Reference to the map
     * @param position Position to be flipped
     * @return DoubleVec2D Flipped position
     */
    static DoubleVec2D flipBotPosition(const Map &map, DoubleVec2D position);

    /**
     * Flips an offset position
     *
     * @param map Reference to the map
     * @param position Offset to be flipped
     * @return Vec2D Flipped offset
     */
    static Vec2D flipOffset(const Map &map, Vec2D position);

    /**
     * Flips a given tower position
     *
     * @param map Reference to the map
     * @param position Position to be flipped
     * @return Vec2D Flipped tower position
     */
    static DoubleVec2D flipTowerPosition(const Map &map, DoubleVec2D position);

  public:
    StateSyncer();

    StateSyncer(std::unique_ptr<ICommandTaker> state,
                std::unique_ptr<ICommandGiver> command_giver,
                logger::ILogger *logger);

    /**
     * @see IStateSyncer #UpdateMainState
     */
    void updateMainState(std::array<player_state::State, 2> &player_states,
                         std::array<bool, 2> skip_turns) override;

    /**
     * @see IStateSyncer #UpdatePlayerStates
     */
    void updatePlayerStates(
        std::array<player_state::State, 2> &player_states) override;

    /**
     * @see IStateSyncer #GetScores
     */
    std::array<uint64_t, 2> getScores() const override;

    /**
     * Function to the assign player state bots their new states after
     * validation of user's actions
     *
     * @param player_id Player id in state
     * @param player_bots Given player's player state bots
     * @param is_enemy Whether the bots are enemy bots or that player's bots
     */
    void assignBots(int64_t player_id,
                    std::vector<player_state::Bot> &player_bots, bool is_enemy);

    /**
     *  Function to the assign player state towers their new states after
     * validation of user's actions
     *
     * @param player_id Player id in state
     * @param player_towers Given player's player state towers
     * @param is_enemy Whether the towers are enemy towers or that player's
     * towers
     */
    void assignTowers(int64_t player_id,
                      std::vector<player_state::Tower> &player_towers,
                      bool is_enemy);

    /**
     *  Returns the player's id
     *
     * @param player_id Id struct
     * @param is_enemy Whether this is the id of the enemy
     * @return int64_t Player id
     */
    size_t getPlayerId(size_t id, bool is_enemy) const;

    /**
     * Returns corresponding bot position given tower position
     *
     * @param position Tower position
     * @return DoubleVec2D Bot position
     */
    DoubleVec2D changeTowerToBotPosition(Vec2D position);

    /**
     * Returns corresponding bot position given the tower position
     *
     * @param position Bot position
     * @return Vec2D Tower position
     */
    DoubleVec2D changeBotToTowerPosition(DoubleVec2D position);
};

} // namespace state
