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
    int player_id_1 = (int)PlayerId::PLAYER1,
        player_id_2 = (int)PlayerId::PLAYER2;
    if (no_of_bots[player_id_1] == no_of_bots[player_id_2])
        return;
    int dominant_player_id = no_of_bots[player_id_1] > no_of_bots[player_id_2]
                                 ? player_id_1
                                 : player_id_2;
    scores[dominant_player_id]++;
}

std::array<size_t, 2> ScoreManager::getScores() { return scores; }
} // namespace state
