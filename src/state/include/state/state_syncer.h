/**
 * @file state_syncer.h
 * Declares the StateSyncer class
 */

#pragma once

#include "logger/interfaces/i_logger.h"
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
    void assignBots(std::vector<player_state::Bot> player_bots, bool is_enemy);

    /**
     *  Function to the assign player state towers their new states after
     * validation of user's actions
     */
    void assignTowers(std::vector<player_state::Tower> player_towers,
                      bool is_enemy);

    /**
     * Helper function to get the player id
     */
    size_t getPlayerId(size_t player_id, bool is_enemy) const;
};

} // namespace state
