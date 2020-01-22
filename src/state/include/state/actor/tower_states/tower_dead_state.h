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
  void Enter() override;

  /**
   * Performs state transitions
   * Finds all enemy actors within attack range, decreases their hp and
   * transitions into the DEAD_STATE
   *
   * @return A unique pointer to the next state that the tower transitions into
   */
  std::unique_ptr<IActorState> Update() override;

  /**
   * Called right before the tower switches from this state to another state
   */
  void Exit() override;
};
} // namespace state