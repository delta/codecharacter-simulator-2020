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
     * Get points based on number of bots and towers owned by the player
     *
     * @param player_id
     *
     * @return double
     */
    double getPlayerPoints(PlayerId player_id) const;

  public:
    /**
     * Constructors
     *
     */
    ScoreManager();

    ScoreManager(std::array<size_t, 2> scores);

    /**
     * Record actor entering flag
     *
     * @param player_id
     */
    void actorEnteredFlagArea(ActorType actor_type, PlayerId player_id);

    /**
     * Record actor exiting flag
     *
     * @param player_id
     */
    void actorExitedFlagArea(ActorType actor_type, PlayerId player_id);

    /**
     * Update the score
     * This function is called after every frame
     */
    void updateScores();

    /**
     * Returns the scores
     *
     * @return std::array<size_t, 2> scores
     */
    std::array<size_t, 2> getScores() const;

    /**
     * Get the Tower Counts at the end of the turn
     *
     * @return std::array<size_t, 2>
     */
    std::array<size_t, 2> getTowerCounts() const;

    /**
     * Gets the Bot Counts at the end of the turn
     *
     * @return std::array<size_t, 2>
     */
    std::array<size_t, 2> getBotCounts() const;
};
} // namespace state
