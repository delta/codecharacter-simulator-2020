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
    int64_t max = 0;
    int dominant_player = -1;
    for (int i = 0; i < (int)PlayerId::PLAYER_COUNT; i++) {
        if (max == no_of_bots[i])
            dominant_player = -1;
        else if (max < no_of_bots[i]) {
            dominant_player = i;
            max = no_of_bots[i];
        }
    }
    scores[dominant_player]++;
}

std::array<size_t, 2> ScoreManager::getScores() { return scores; }
} // namespace state
