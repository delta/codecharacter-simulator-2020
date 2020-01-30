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
    std::array<size_t, 2> num_bots;

    /**
     * Number of towers of players in flag area
     */
    std::array<size_t, 2> num_towers;

    /**
     * Get the Individual Points based on number of player bots and towers
     *
     * @param player_id
     *
     * @return size_t
     */
    size_t getIndividualPoints(PlayerId player_id) const;

  public:
    ScoreManager(std::array<size_t, 2> scores);

    /**
     * Record bot entering flag
     *
     * @param player_id
     */
    void botEnteredFlagArea(PlayerId player_id);

    /**
     * Record bot exiting flag
     *
     * @param player_id
     */
    void botExitedFlagArea(PlayerId player_id);

    /**
     * Record tower entering flag
     *
     * @param player_id
     */
    void towerEnteredFlagArea(PlayerId player_id);

    /**
     * Record tower exiting flag
     *
     * @param player_id
     */
    void towerExitedFlagArea(PlayerId player_id);

    /**
     * Update the score
     * This function is called after every frame
     */
    void updateScore();

    /**
     * Returns the scores
     *
     * @return std::array<size_t, PlayerId::PLAYER_COUNT> scores
     */
    std::array<size_t, 2> getScores() const;
};
} // namespace state
