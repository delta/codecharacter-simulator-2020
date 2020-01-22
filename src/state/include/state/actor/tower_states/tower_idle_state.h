/**
 * @file tower_idle_state.h
 * Declares a class describing the tower's behaviour in the idle state
 */

#pragma once

#include "state/actor/interfaces/i_actor_state.h"
#include "state/actor/tower.fwd.h"
#include "state/actor/tower_states/tower_state.h"

namespace state {
class STATE_EXPORT TowerIdleState : public TowerState {
public:
  /**
   * Constructor
   */
  TowerIdleState();

  TowerIdleState(Tower *tower);

  /**
   * Called right after the tower switches to this state
   */
  void Enter() override;

  /**
   * Performs state transitions
   * If the tower blasts, then the state will transition to blast state and dead
   * state fron blast state
   * Else, remain in idle state
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