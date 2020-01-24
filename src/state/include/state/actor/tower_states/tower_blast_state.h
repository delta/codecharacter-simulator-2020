/**
 * @file tower_blast_state.h
 * Declares a class describing the tower's behaviour in the blast state
 */

#pragma once

#include "state/actor/interfaces/i_actor_state.h"
#include "state/actor/tower_states/tower_state.h"

namespace state {
class STATE_EXPORT TowerBlastState : public TowerState {
  public:
    /**
     * Constructor
     */
    TowerBlastState();

    TowerBlastState(Tower *tower);

    /**
     * Called right after the tower switches to this state
     */
    void enter() override;

    /**
     * Performs state transitions
     * Finds all enemy actors within attack range, decreases their hp and
     * transitions into the DEAD_STATE
     *
     * @return A unique pointer to the next state that the tower transitions
     * into
     */
    std::unique_ptr<IActorState> update() const override;

    /**
     * Called right before the tower switches from this state to another state
     */
    void exit() override;
};
} // namespace state
