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

void TowerDeadState::enter() {}

void TowerDeadState::exit() {}

std::unique_ptr<IActorState> TowerDeadState::update() const { return nullptr; }
} // namespace state
