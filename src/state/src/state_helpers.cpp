/**
 * @file state_helpers.cpp
 *
 * Contains extra helper functions for state
 */

#include "state/state.h"

namespace state {

Bot *State::getBotById(PlayerId player_id, ActorId actor_id) {
    int64_t id = static_cast<int64_t>(player_id);

    for (auto &bot : bots[id]) {
        if (bot->getActorId() == actor_id) {
            return bot.get();
        }
    }

    return nullptr;
}

Tower *State::getTowerById(PlayerId player_id, ActorId actor_id) {
    int64_t id = static_cast<int64_t>(player_id);

    for (auto &tower : towers[id]) {
        if (tower->getActorId() == actor_id) {
            return tower.get();
        }
    }

    return nullptr;
}

Blaster *State::getBlasterById(PlayerId player_id, ActorId actor_id) {
    // Checking if actor is a bot
    Bot *bot = getBotById(player_id, actor_id);

    if (bot) {
        return bot;
    }

    // Checking if actor is a tower
    Tower *tower = getTowerById(player_id, actor_id);
    return tower;
}

Actor *State::getActorById(PlayerId player_id, ActorId actor_id) {
    // Checking if actor is a bot
    Bot *bot = getBotById(player_id, actor_id);

    if (bot) {
        return bot;
    }

    // Checking if actor is a tower
    Tower *tower = getTowerById(player_id, actor_id);
    return tower;
}

Actor *State::getActorByPosition(DoubleVec2D position) {
    for (int player_id = 0;
         player_id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++player_id) {
        for (auto &bot : bots[player_id]) {
            if (bot->getPosition() == position) {
                return bot.get();
            }
        }

        for (auto &tower : towers[player_id]) {
            if (tower->getPosition() == position) {
                return tower.get();
            }
        }

        return nullptr;
    }
}

} // namespace state
