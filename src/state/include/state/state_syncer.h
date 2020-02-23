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
    std::unique_ptr<logger::ILogger> logger;

  public:
    StateSyncer();

    StateSyncer(std::unique_ptr<ICommandTaker> state,
                std::unique_ptr<ICommandGiver> command_giver,
                std::unique_ptr<logger::ILogger> logger);

    /**
     * @see IStateSyncer #UpdateMainState
     */
    void updateMainState(std::array<player_state::State, 2> &player_states,
                         std::array<bool, 2> skip_turns) override;

    /**
     * @see IStateSyncer #UpdatePlayerStates
     */
    void updatePlayerStates(std::array<player_state::State, 2> &player_states);

    /**
     * @see IStateSyncer #IsGameOver
     */
    bool isGameOver(PlayerId &winner);

    /**
     * @see IStateSyncer #GetScores
     */
    std::array<int64_t, 2> getScores(bool game_over) override;

    /**
     *  Function to the assign player state bots their new states after
     * validation of user's actions
     */
    void assignBots(size_t id, std::vector<player_state::Bot> player_bots,
                    bool is_enemy);

    /**
     *  Function to the assign player state towers their new states after
     * validation of user's actions
     */
    void assignTowers(size_t id, std::vector<player_state::Tower> player_towers,
                      bool is_enemy);

    /**
     * Helper function to get the player id
     */
    size_t getPlayerId(size_t id, bool is_enemy) const;
     
    /**
     * @param position Position that needs to be flipped
     * @return DoubleVec2D Flipped position
     */
    DoubleVec2D flipBotPosition(const Map *map, DoubleVec2D position);

    /**
     * Returns flipped tower position
     *
     * @param position Position that needs to be flipped
     * @return Vec2D Flipped position
     */
    Vec2D flipTowerPosition(const Map *map, Vec2D position);

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
    Vec2D changeBotToTowerPosition(DoubleVec2D position);
};

} // namespace state
