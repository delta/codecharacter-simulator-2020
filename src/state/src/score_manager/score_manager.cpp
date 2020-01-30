/**
 * @file scoremanager.cpp
 */

#include "state/score_manager/score_manager.h"
#include "constants/constants.h"
#include "state/utilities.h"

namespace state {

ScoreManager::ScoreManager(std::array<size_t, 2> scores)
    : scores(scores), num_bots({0, 0}), num_towers({0, 0}) {}

double ScoreManager::getIndividualPoints(PlayerId player_id) const {
    using namespace Constants::Score;
    double player_points = 0;
    player_points +=
        BOT_SCORE_MULTIPLIER * num_bots[static_cast<size_t>(player_id)];
    player_points +=
        TOWER_SCORE_MULTIPLIER * num_towers[static_cast<size_t>(player_id)];
    return player_points;
}

void ScoreManager::botEnteredFlagArea(PlayerId player_id) {
    num_bots[static_cast<size_t>(player_id)]++;
}

void ScoreManager::botExitedFlagArea(PlayerId player_id) {
    num_bots[static_cast<size_t>(player_id)]--;
}

void ScoreManager::towerEnteredFlagArea(PlayerId player_id) {
    num_towers[static_cast<size_t>(player_id)]++;
}

void ScoreManager::towerExitedFlagArea(PlayerId player_id) {
    num_towers[static_cast<size_t>(player_id)]--;
}

void ScoreManager::updateScore() {

    auto points_1 = getIndividualPoints(PlayerId::PLAYER1);
    auto points_2 = getIndividualPoints(PlayerId::PLAYER2);

    if (points_1 == points_2)
        return;
    auto dominant_player_id =
        points_1 > points_2 ? PlayerId::PLAYER1 : PlayerId::PLAYER2;
    scores[static_cast<size_t>(dominant_player_id)]++;
}

std::array<size_t, 2> ScoreManager::getScores() const { return scores; }
} // namespace state
