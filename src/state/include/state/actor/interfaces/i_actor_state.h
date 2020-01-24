/**
 * @file i_actor_state.h
 * Declares an interface for actor states
 */

#include "state/state_export.h"
#include <memory>

#pragma once

namespace state {

class STATE_EXPORT IActorState {
  public:
    virtual ~IActorState() {}

    /**
     * Called right after the actor switches to this state
     */
    virtual void enter() = 0;

    /**
     * Executes state code when called
     * Returns the next actor state when a state transition occurs
     * Returns nullptr if no state transition occurs (no transition)
     *
     * @return      A pointer to the new state
     */
    virtual std::unique_ptr<IActorState> update() const = 0;

    /**
     * Called before the actor switches to another state
     */
    virtual void exit() = 0;
};
} // namespace state
