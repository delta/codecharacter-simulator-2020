/**
 * @file state_syncer.h
 * Declares the StateSuyncer class
 */

#pragma once

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
  void UpdateMainState(std::array<player_state::State, 2> &player_states,
                       std::array<bool, 2> skip_turns);

  /**
   * @see IStateSyncer #UpdatePlayerStates
   */
  void UpdatePlayerStates(std::array<player_state::State, 2> &player_states);

  /**
   * @see IStateSyncer #IsGameOver
   */
  bool isGameOver(PlayerId &winner);

  /**
   * @see IStateSyncer #GetScores
   */
  std::array<int64_t, 2> GetScores(bool game_over);

  /**
   *  Function to the player state bots their new states after validation of
   * user's actions
   */
  void AssignBots(vector<player_state::Bot> player_bots, bool is_enemy);

  /**
   * Helper function to get the player id
   */
  int64_t GetPlayerId(int id, bool is_enemy);
};

} // namespace state
