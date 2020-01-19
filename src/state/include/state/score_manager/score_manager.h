/**
 * @file score_manager.h
 */

#pragma once

#include "state/state_export.h"
#include "state/utilities.h"

#include <array>

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

  public:
    ScoreManager(std::array<size_t, 2> scores, size_t flag_area_enter_reward,
                 size_t flag_area_exit_reward);

    /**
     * Rewards for entering flag area
     *
     * @param player_id
     */
    void botEnteredFlagArea(PlayerId player_id);

    /**
     * Rewards for exiting flag area
     *
     * @param player_id
     */
    void botExitedFlagArea(PlayerId player_id);

    /**
     * Returns the scores
     *
     * @return std::array<size_t, 2> scores
     */
    std::array<size_t, 2> getScores();
};
} // namespace state
