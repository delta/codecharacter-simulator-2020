/**
 * @file tower.h
 * Declares the base tower class
 */

#pragma once

#include "state/actor/blaster.h"
#include "state/actor/tower_states/tower_state.h"
#include <memory>

namespace state {
class STATE_EXPORT Tower : public Blaster {
private:
  /**
   * Controls logic for tower's current state
   */
  std::unique_ptr<TowerState> state;

  /**
   * True if the tower is going to blast
   * False if the tower isn't blasting
   */
  bool blast;

public:
  /**
   * Constructors
   */
  Tower(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
        size_t max_hp, size_t damage_incurred, Vec2D pos,
        std::unique_ptr<TowerState> state);

  /**
   * @see Blaster#Blast
   */
  void Blast() override;

  /**
   * Helper function to check if the tower is going to blast
   *
   * @return true if the tower is blasting
   * @return false if the tower is not blasting
   */
  bool IsBlasting();

  /**
   * Returns the name of the state that the tower is currently in
   *
   * @return The name of the tower's state
   */
  TowerStateName GetState();

  /**
   * Updates the state of the factory and all related properties
   */
  void Update() override;

  /**
   * Performs late updates for the factory
   */
  void LateUpdate() override;
};

} // namespace state