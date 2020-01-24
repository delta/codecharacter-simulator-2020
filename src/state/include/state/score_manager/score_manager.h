/**
 * @file score_manager.h
 */

#pragma once

#include "state/state_export.h"
#include "state/utilities.h"

#include <array>
#include <cstdint>

namespace state {

/**
 * Score Manager class to manage the scores of player
 */
class STATE_EXPORT ScoreManager {

  private:
    /**
     * Current game scores
     */
    std::array<size_t, 2> scores;

    /**
     * Reward for entering flag area
     */
    size_t flag_area_enter_reward;

    /**
     * Reward for exit flag area
     */
    size_t flag_area_exit_reward;

    /**
     * Number of bots of player 1 in flag area
     */
    int64_t no_player_1_bots;

    /**
     * Number of bots of player 2 in flag area
     */
    int64_t no_player_2_bots;

  public:
    ScoreManager(std::array<size_t, 2> scores, size_t flag_area_enter_reward,
                 size_t flag_area_exit_reward);

    /**
     * Record bot leaving
     *
     * @param player_id
     */
    void botEnteredFlagArea(PlayerId player_id);

    /**
     * Record bot entering
     *
     * @param player_id
     */
    void botExitedFlagArea(PlayerId player_id);

    /**
     * Update the score
     * Call this function after simulating ervery frame
     */
    void updateScore();

    /**
     * Returns the scores
     *
     * @return std::array<size_t, 2> scores
     */
    std::array<size_t, 2> getScores();
};
} // namespace state
