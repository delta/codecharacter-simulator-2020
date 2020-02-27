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
    }

    return nullptr;
}

} // namespace state
