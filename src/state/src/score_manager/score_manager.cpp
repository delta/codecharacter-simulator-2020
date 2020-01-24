/**
 * @file scoremanager.cpp
 */

#include "state/score_manager/score_manager.h"

namespace state {

ScoreManager::ScoreManager(std::array<size_t, 2> scores)
    : scores(scores), no_of_bots({0, 0}) {}

void ScoreManager::botEnteredFlagArea(PlayerId player_id) {
    no_of_bots[(int)player_id]++;
}

void ScoreManager::botExitedFlagArea(PlayerId player_id) {
    no_of_bots[(int)player_id]--;
}

void ScoreManager::updateScore() {
    if (no_of_bots[(int)PlayerId::PLAYER1] > no_of_bots[(int)PlayerId::PLAYER2])
        scores[(int)PlayerId::PLAYER1]++;

    else if (no_of_bots[(int)PlayerId::PLAYER2] >
             no_of_bots[(int)PlayerId::PLAYER1])
        scores[(int)PlayerId::PLAYER2]++;
}

std::array<size_t, 2> ScoreManager::getScores() { return scores; }
} // namespace state
