/**
 * @file tower_blast_state.cpp
 * Defines the functions member functions responsible for how a tower operates
 * in the blast state
 */

#include "state/actor/tower_states/tower_blast_state.h"
#include "state/actor/tower.h"
#include "state/actor/tower_states/tower_dead_state.h"

namespace state {
TowerBlastState::TowerBlastState() {}

TowerBlastState::TowerBlastState(Tower *tower)
    : TowerState(TowerStateName::BLAST, tower) {}

void TowerBlastState::Enter() {
  size_t max_tower_hp = tower->getMaxHp();
  tower->damage(max_tower_hp);
}

void TowerBlastState::Exit() {}

std::unique_ptr<IActorState> TowerBlastState::Update() {
  if (tower->getHp() == 0) {
    return std::make_unique<TowerDeadState>(tower);
  }

  return nullptr;
}
} // namespace state