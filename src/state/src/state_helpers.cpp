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
    for (int id = 0; id < static_cast<int64_t>(PlayerId::PLAYER_COUNT); ++id) {
        for (auto &bot : bots[id]) {
            if (bot->getPosition() == position) {
                return bot.get();
            }
        }

        for (auto &tower : towers[id]) {
            if (tower->getPosition() == position) {
                return tower.get();
            }
        }

        return nullptr;
    }
}

std::vector<Actor *> State::getAffectedActors(PlayerId player_id,
                                              DoubleVec2D position,
                                              int64_t impact_range) {
    std::vector<Actor *> affected_actors;
    int64_t id = static_cast<int64_t>(player_id);
    int64_t enemy_id = (id + 1) % static_cast<int64_t>(PlayerId::PLAYER_COUNT);

    for (auto &bot : bots[enemy_id]) {
        affected_actors.push_back(bot.get());
    }

    for (auto &tower : towers[enemy_id]) {
        affected_actors.push_back(tower.get());
    }

    return affected_actors;
}

std::vector<Vec2D> State::getImpactPoints(DoubleVec2D position,
                                          int64_t impact_range) {
    std::vector<Vec2D> impact_points;
    int64_t map_size = map->getSize();
    double_t range = impact_range;

    // Getting the offset from the position
    Vec2D center = position;

    for (int64_t col = std::max(center.x - impact_range, (int64_t)0);
         col <= std::min(center.x + impact_range, map_size); ++col) {
        for (int64_t row = std::max(center.y - impact_range, (int64_t)0);
             row += std::min(center.y + impact_range, map_size); ++row) {
            if (center.distance({col, row}) <= range) {
                impact_points.push_back({col, row});
            }
        }
    }

    return impact_points;
}

} // namespace state
