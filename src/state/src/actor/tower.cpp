/**
 * @file tower.cpp
 * Defines the tower class
 */

#include "state/actor/tower.h"

#include <memory>

namespace state {

Tower::Tower(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
             size_t max_hp, Vec2D pos, size_t damage_points, size_t blast_range,
             BlastCallback blast_callback)

    : Actor(id, player_id, actor_type, hp, max_hp, pos),
      Blaster(blast_range, damage_points, std::move(blast_callback)),
      state(std::make_unique<TowerIdleState>(this)) {}

Tower::Tower(PlayerId player_id, ActorType actor_type, size_t hp, size_t max_hp,
             Vec2D pos, size_t damage_points, size_t blast_range,
             BlastCallback blast_callback)
    : Actor(player_id, actor_type, hp, max_hp, pos),
      Blaster(blast_range, damage_points, std::move(blast_callback)),
      state(std::make_unique<TowerIdleState>(this)) {}

void Tower::blast() { blasting = true; }

void Tower::damageEnemyActors() { blast_callback(player_id, position); }

TowerStateName Tower::getState() { return state->getName(); }

void Tower::update() {
    auto new_state = state->update();

    // Keep transitioning states until there are no more transitions
    while (new_state != nullptr) {
        state->exit();
        /*
            Here, state.reset destroys the state it is currently managing and
           starts managing the new state object passed to it
        */
        state.reset(static_cast<TowerState *>(new_state.release()));
        state->enter();
        new_state = state->update();
    }
}

void Tower::lateUpdate() {
    // Updating the hp of the tower
    size_t latest_hp = getLatestHp();
    setHp(latest_hp);

    // Resetting the damage incurred
    setDamageIncurred(0);

    auto new_state = state->update();

    while (new_state != nullptr) {
        state->exit();
        /*
            Here, state.reset destroys the state it is currently managing and
           starts managing the new state object passed to it
        */
        state.reset(static_cast<TowerState *>(new_state.release()));
        state->enter();
        new_state = state->update();
    }
}
} // namespace state
