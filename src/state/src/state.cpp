/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#pragma once

#include "state/state.h"

namespace state {
State::State() {}

State::State(std::unique_ptr<Map> map,
             std::unique_ptr<ScoreManager> score_manager,
             std::array<std::vector<std::unique_ptr<Bot>>, 2> bots,
             std::array<std::vector<std::unique_ptr<Tower>>, 2> towers)
    : map(std::move(map)), score_manager(std::move(score_manager)),
      bots(std::move(bots)), towers(std::move(towers)) {}

const Map *State::getMap() const { return map.get(); }

const std::array<size_t, 2> State::getScores(bool game_over) const {
    return score_manager->getScores();
}

/**
 * Get the Raw Ptrs From Unique Ptrs object
 *
 * @tparam T Actor type that needs to be extracted
 * @param actors The actors' vectors
 * @return const std::array<std::vector<T *>, 2>
 */
template <typename T>
const std::array<std::vector<T *>, 2> getRawPtrsFromUniquePtrs(
    std::array<std::vector<std::unique_ptr<T>>, 2> &actors) {
    std::array<std::vector<T *>, 2> ret_actors;

    for (int id = 0; id < static_cast<size_t>(PlayerId::PLAYER_COUNT); ++id) {
        std::vector<T *> actor_row;
        actor_row.resize(actors[id].size());

        for (int actor_index = 0; actor_index < actors[id].size();
             ++actor_index) {
            actor_row.push_back(actors[id][actor_index].get());
        }
        ret_actors[id] = actor_row;
    }
    return ret_actors;
}

bool State::isGameOver(PlayerId &winner) {}

void State::moveBot(PlayerId player_id, ActorId actor_id,
                    DoubleVec2D position) {
                        // Setting the bot's destination
                        auto bot = getActorById(player_id, actor_id);
                        // bot->setDestination(position);
                    }

void State::transformBot(PlayerId player_id, ActorId bot_id,
                         DoubleVec2D position) {
                             // Creating a transform request
                             
                         }

void State::blastActor(PlayerId player_id, ActorId actor_id,
                       DoubleVec2D position) {
    Blaster* blaster = getBlasterById(player_id, actor_id);
    Actor *actor = getActorById(player_id, actor_id);

    int64_t impact_radius = blaster->getBlastRange();
    int64_t damage_points = blaster->getBlastDamage();

    // Getting actors around position of size impact radius
    std::vector<Actor *> affected_actors = getAffectedActors(player_id, position, impact_radius);

    // Adding to the actor's damange incurred
    for(auto actor : affected_actors){
        actor->damage(damage_points);
    }
}

const std::array<std::vector<Tower *>, 2> State::getTowers() {
    return getRawPtrsFromUniquePtrs(towers);
}

const std::array<std::vector<Bot *>, 2> State::getBots() {
    return getRawPtrsFromUniquePtrs(bots);
}

} // namespace state
