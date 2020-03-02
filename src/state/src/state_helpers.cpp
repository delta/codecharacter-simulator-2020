/**
 * @file state_helpers.cpp
 *
 * Contains extra helper functions for state
 */

#include "state/state.h"

namespace state {

Bot *State::getBotById(ActorId actor_id) {
    for (int64_t id = 0; id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++id) {
        for (auto &bot : bots[id]) {
            if (bot->getActorId() == actor_id) {
                return bot.get();
            }
        }
    }

    return nullptr;
}

Tower *State::getTowerById(ActorId actor_id) {
    for (int64_t id = 0; id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++id) {
        for (auto &tower : towers[id]) {
            if (tower->getActorId() == actor_id) {
                return tower.get();
            }
        }
    }

    return nullptr;
}

Blaster *State::getBlasterById(ActorId actor_id) {
    // Checking if actor is a bot
    Bot *bot = getBotById(actor_id);

    if (bot) {
        return bot;
    }

    // Checking if actor is a tower
    Tower *tower = getTowerById(actor_id);
    return tower;
}

Actor *State::getActorById(ActorId actor_id) {
    // Checking if actor is a bot
    Bot *bot = getBotById(actor_id);

    if (bot) {
        return bot;
    }

    // Checking if actor is a tower
    Tower *tower = getTowerById(actor_id);
    return tower;
}

std::vector<Vec2D> State::getNeighbouringPoints(DoubleVec2D position) {
    uint64_t pos_x = std::floor(position.x), pos_y = std::floor(position.y);

    if (pos_x == position.x) {
        pos_x = std::min(pos_x - 1, (uint64_t) 0);
    }
    if (pos_y == position.y) {
        pos_y = std::min(pos_y - 1, (uint64_t) 0);
    }

    std::vector<Vec2D> neighbouring_points;
    for (auto x = pos_x; x < std::floor(position.x); ++x) {
        for (auto y = pos_y; y < std::floor(position.y); ++y) {
            neighbouring_points.push_back({pos_x, pos_y});
        }
    }

    return neighbouring_points;
}

} // namespace state
