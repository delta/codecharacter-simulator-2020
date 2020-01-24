/**
 * @file tower_state.cpp
 * Defines the functions for the class TowerState
 */

#include "state/actor/tower_states/tower_state.h"

namespace state {
TowerState::TowerState() {}

TowerState::TowerState(TowerStateName tower_state_name, Tower *tower)
    : state(tower_state_name), tower(tower) {}

TowerStateName TowerState::getName() { return state; }
} // namespace state
