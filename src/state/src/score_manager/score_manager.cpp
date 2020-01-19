/**
 * @file scoremanager.cpp
 */

#include "state/score_manager/score_manager.h"

namespace state {

ScoreManager::ScoreManager(std::array<size_t, 2> scores,
                           size_t flag_area_enter_reward,
                           size_t flag_area_exit_reward)
    : scores(scores), flag_area_enter_reward(flag_area_enter_reward),
      flag_area_exit_reward(flag_area_exit_reward) {}

void ScoreManager::botEnteredFlagArea(PlayerId player_id) {
    scores[(int)player_id] += flag_area_enter_reward;
}

void ScoreManager::botExitedFlagArea(PlayerId player_id) {
    scores[(int)player_id] += flag_area_exit_reward;
}

std::array<size_t, 2> ScoreManager::getScores() { return scores; }
} // namespace state
