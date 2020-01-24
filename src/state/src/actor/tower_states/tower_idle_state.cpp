/**
 * @file tower_idle_state.cpp
 * Defines the member functions resposible for how a tower behaves in the idle
 * state
 */

#include "state/actor/tower_states/tower_idle_state.h"
#include "state/actor/tower.h"
#include "state/actor/tower_states/tower_blast_state.h"
#include "state/actor/tower_states/tower_dead_state.h"

#include <memory>

namespace state {
TowerIdleState::TowerIdleState() {}

TowerIdleState::TowerIdleState(Tower *tower)
    : TowerState(TowerStateName::IDLE, tower) {}

void TowerIdleState::enter() {}

std::unique_ptr<IActorState> TowerIdleState::update() const {
    // If the tower hp is 0, transition to the dead state
    if (tower->getHp() == 0) {
        return std::make_unique<TowerDeadState>(tower);
    }

    // If the tower is blasting, transitioning to the blast state
    if (tower->isBlasting()) {
        return std::make_unique<TowerBlastState>(tower);
    }

    return nullptr;
}

void TowerIdleState::exit() {}
} // namespace state
