/**
 * @file tower.cpp
 * Defines the tower class
 */

#include "state/actor/tower.h"

#include <memory>
#include <state/actor/tower_states/tower_blast_state.h>
#include <state/actor/tower_states/tower_dead_state.h>

namespace state {

Tower::Tower(ActorId id, PlayerId player_id, size_t hp, size_t max_hp,
             DoubleVec2D position, size_t damage_points, size_t blast_range,
             ScoreManager *score_manager, BlastCallback blast_callback)
    : Actor(id, player_id, ActorType::TOWER, hp, max_hp, position,
            score_manager),
      Blaster(blast_range, damage_points, std::move(blast_callback)),
      state(std::make_unique<TowerIdleState>(this)), age(0) {}

Tower::Tower(PlayerId player_id, size_t hp, size_t max_hp, DoubleVec2D position,
             size_t damage_points, size_t blast_range,
             ScoreManager *score_manager, BlastCallback blast_callback)
    : Actor(player_id, ActorType::TOWER, hp, max_hp, position, score_manager),
      Blaster(blast_range, damage_points, std::move(blast_callback)),
      state(std::make_unique<TowerIdleState>(this)), age(0) {}

Tower::Tower(ActorId id, PlayerId player_id, TowerStateName tower_state,
             size_t hp, size_t max_hp, DoubleVec2D position,
             size_t damage_points, size_t blast_range,
             BlastCallback blast_callback, ScoreManager *score_manager,
             bool is_blasting)
    : Actor(id, player_id, ActorType::TOWER, hp, max_hp, position,
            score_manager),
      Blaster(blast_range, damage_points, is_blasting,
              std::move(blast_callback)),
      age(0) {

    switch (tower_state) {
    case TowerStateName::IDLE:
        state = std::make_unique<TowerIdleState>(this);
        break;
    case TowerStateName::BLAST:
        state = std::make_unique<TowerBlastState>(this);
        break;
    case TowerStateName::DEAD:
        state = std::make_unique<TowerDeadState>(this);
        break;
    }
}

void Tower::blast() { setBlasting(true); }

TowerStateName Tower::getState() { return state->getName(); }

uint64_t Tower::getAge() const { return age; }

void Tower::incrementAge() { age++; }

void Tower::update() {
    // Increase age of tower when turn starts
    incrementAge();

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
    setHp(getLatestHp());

    // Resetting the damage incurred
    setDamageIncurred(0);

    // Allow Tower to transition to dead state
    if (getHp() == 0 && state->getName() != TowerStateName::DEAD) {
        auto new_state = state->update();
        state->exit();
        state.reset(static_cast<TowerState *>(new_state.release()));
        state->enter();
    }
}

} // namespace state
