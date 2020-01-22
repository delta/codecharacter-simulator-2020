/**
 * @file tower.cpp
 * Defines the tower class
 */

#include "state/actor/tower.h"

#include <memory>

namespace state {

Tower::Tower(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
             size_t max_hp, size_t damage_incurred, Vec2D pos,
             std::unique_ptr<TowerState> state)
    : Blaster(id, player_id, actor_type, hp, max_hp, damage_incurred, pos),
      state(std::move(state)) {}

bool Tower::IsBlasting() { return blast; }

void Tower::Blast() { blast = true; }

TowerStateName Tower::GetState() { return state->GetName(); }

void Tower::Update() {
  auto new_state = state->Update();

  // Keep transitioning states until there are no more transitions
  while (new_state != nullptr) {
    state->Exit();
    state = std::unique_ptr<TowerState>(
        static_cast<TowerState *>(new_state.release()));
    state->Enter();
    new_state = state->Update();
  }
}

void Tower::LateUpdate() {
  // Updating the hp of the tower
  setHp(hp - damage_incurred);

  // Resetting the damage incurred
  damage(0);

  auto new_state = state->Update();

  while (new_state != nullptr) {
    state->Enter();
    state = std::unique_ptr<TowerState>(
        static_cast<TowerState *>(new_state.release()));
    state->Exit();
    new_state = state->Update();
  }
}
} // namespace state