/**
 * @file scoremanager.cpp
 */

#include "state/score_manager/score_manager.h"

namespace state {

ScoreManager::ScoreManager(std::array<size_t, 2> scores,
                           size_t flag_area_enter_reward,
                           size_t flag_area_exit_reward)
    : scores(scores), flag_area_enter_reward(flag_area_enter_reward),
      flag_area_exit_reward(flag_area_exit_reward), no_player_1_bots(0),
      no_player_2_bots(0) {}

void ScoreManager::botEnteredFlagArea(PlayerId player_id) {
    switch (player_id) {
    case PlayerId::PLAYER1:
        no_player_1_bots++;
        break;
    case PlayerId::PLAYER2:
        no_player_2_bots++;
    }
}

void ScoreManager::botExitedFlagArea(PlayerId player_id) {
    switch (player_id) {
    case PlayerId::PLAYER1:
        no_player_1_bots--;
        break;
    case PlayerId::PLAYER2:
        no_player_2_bots--;
    }
}

void ScoreManager::updateScore() {
    if (no_player_1_bots > no_player_2_bots) {
        scores[(int)PlayerId::PLAYER1]++;
    } else if (no_player_2_bots > no_player_1_bots) {
        scores[(int)PlayerId::PLAYER2]++;
    }
}

std::array<size_t, 2> ScoreManager::getScores() { return scores; }
} // namespace state
