/**
 * @file score_manager.h
 * Manages the scores for the players
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
     * Number of bots of player in flag area
     */
    std::array<size_t, 2> no_of_bots;

  public:
    ScoreManager(std::array<size_t, 2> scores);

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
