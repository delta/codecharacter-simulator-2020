/**
 * @file tower_dead_state.cpp
 * Declares the member functions responsible for how the tower behaves in the
 * dead state
 */

#include "state/actor/tower_states/tower_dead_state.h"
#include "state/actor/tower.fwd.h"

namespace state {
TowerDeadState::TowerDeadState() {}

TowerDeadState::TowerDeadState(Tower *tower)
    : TowerState(TowerStateName::DEAD, tower) {}

void TowerDeadState::Enter() {}

void TowerDeadState::Exit() {}

std::unique_ptr<IActorState> TowerDeadState::Update() { return nullptr; }
} // namespace state