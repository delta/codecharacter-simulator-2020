/**
 * @file tower_state.h
 * Declares an interface for tower_states
 */

#pragma once

#include "state/actor/interfaces/i_actor_state.h"
#include "state/actor/tower.fwd.h"
#include "state/state_export.h"

namespace state {
/**
 * Define a name for each state
 */
enum class TowerStateName {
    // Tower isn't blasting
    IDLE,
    // Tower is blasting
    BLAST,
    // Tower is dead
    DEAD
};

/**
 * The base state that other tower states inherit from
 */
class STATE_EXPORT TowerState : public IActorState {
  protected:
    /**
     * Name of the state
     */
    TowerStateName state;

    /**
     * The tower that this state is managing
     */
    Tower *tower;

  public:
    /**
     * Constructors
     */
    TowerState();

    TowerState(TowerStateName tower_state_name, Tower *tower);

    /**
     * Getter function to return the TowerStateName
     *
     * @return TowerStateName of the tower
     */
    TowerStateName getName();
};
} // namespace state
