/**
 * @file tower_dead_state.h
 * Declares a class describing the tower's behaviour in the dead state
 */

#pragma once

#include "state/actor/interfaces/i_actor_state.h"
#include "state/actor/tower_states/tower_state.h"

namespace state {
class STATE_EXPORT TowerDeadState : public TowerState {
  public:
    /**
     * Constructor
     */
    TowerDeadState();

    TowerDeadState(Tower *tower);

    /**
     * Called right after the tower switches to this state
     */
    void enter() override;

    /**
     * Performs state transitions
     * Simply returns a null pointer and this actor is removed by the end of
     * this turn
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
