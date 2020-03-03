/**
 * @file scoremanager.cpp
 */

#include "state/score_manager/score_manager.h"
#include "constants/constants.h"
#include "state/utilities.h"

namespace state {

ScoreManager::ScoreManager()
    : scores({0, 0}), num_bots({0, 0}), num_towers({0, 0}),
      turn_winner(PlayerId::PLAYER_NULL) {}

ScoreManager::ScoreManager(std::array<size_t, 2> scores)
    : scores(scores), num_bots({0, 0}), num_towers({0, 0}),
      turn_winner(PlayerId::PLAYER_NULL) {}

double ScoreManager::getPlayerPoints(PlayerId player_id) const {
    using namespace Constants::Score;
    double player_points = 0;
    player_points +=
        BOT_SCORE_MULTIPLIER * num_bots[static_cast<size_t>(player_id)];
    player_points +=
        TOWER_SCORE_MULTIPLIER * num_towers[static_cast<size_t>(player_id)];
    return player_points;
}

void ScoreManager::actorEnteredFlagArea(ActorType actor_type,
                                        PlayerId player_id) {
    size_t id = static_cast<size_t>(player_id);
    switch (actor_type) {
    case ActorType::BOT:
        num_bots[id]++;
        break;
    case ActorType::TOWER:
        num_towers[id]++;
        break;
    }
}

std::array<size_t, 2> ScoreManager::getTowerCounts() const {
    return num_towers;
}

std::array<size_t, 2> ScoreManager::getBotCounts() const { return num_bots; }

void ScoreManager::actorExitedFlagArea(ActorType actor_type,
                                       PlayerId player_id) {
    int64_t id = static_cast<size_t>(player_id);
    switch (actor_type) {
    case ActorType::BOT:
        num_bots[id]--;
        break;
    case ActorType::TOWER:
        num_towers[id]--;
        break;
    }
}

void ScoreManager::updateScores() {

    auto points_1 = getPlayerPoints(PlayerId::PLAYER1);
    auto points_2 = getPlayerPoints(PlayerId::PLAYER2);

    if (points_1 == points_2) {
        turn_winner = PlayerId::PLAYER_NULL;
        return;
    }
    turn_winner = points_1 > points_2 ? PlayerId::PLAYER1 : PlayerId::PLAYER2;
    scores[static_cast<size_t>(turn_winner)]++;
}

PlayerId ScoreManager::getTurnWinner() const { return turn_winner; }

std::array<size_t, 2> ScoreManager::getScores() const { return scores; }
} // namespace state
