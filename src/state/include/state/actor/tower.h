/**
 * @file tower.h
 * Declares the base tower class
 */

#pragma once

#include "state/actor/blaster.h"
#include "state/actor/tower_states/tower_idle_state.h"
#include "state/actor/tower_states/tower_state.h"
#include <memory>

namespace state {
class STATE_EXPORT Tower : public Actor, public Blaster {
  private:
    /**
     * Controls logic for tower's current state
     */
    std::unique_ptr<TowerState> state;

    /**
     * Number of turns for which the tower has been alive
     */
    uint64_t age;

  public:
    /**
     *  Construct a new Tower object
     *
     * @param id
     * @param player_id
     * @param actor_type
     * @param hp
     * @param max_hp
     * @param position
     * @param damage_points
     * @param blast_range
     * @param blast_callback
     */
    Tower(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
          size_t max_hp, DoubleVec2D position, size_t damage_points,
          size_t blast_range, BlastCallback blast_callback);

    /**
     *  Construct a new Tower object, with auto incrementing id
     *
     * @param player_id
     * @param actor_type
     * @param hp
     * @param max_hp
     * @param position
     * @param damage_points
     * @param blast_range
     * @param blast_callback
     */
    Tower(PlayerId player_id, ActorType actor_type, size_t hp, size_t max_hp,
          DoubleVec2D position, size_t damage_points, size_t blast_range,
          BlastCallback blast_callback);

    /**
     * @see Blaster#Blast
     */
    void blast() override;

    /**
     * Function that uses callback function to damage enemy units due to
    blasting
     */
    void damageEnemyActors();

    /**
     * Returns the name of the state that the tower is currently in
     *
     * @return The name of the tower's state
     */
    TowerStateName getState();

    /**
     * Return age of the tower
     * @return age
     */
    uint64_t getAge() const;

    /**
     * Increment age of tower by 1
     */
    void incrementAge();

    /**
     * Updates the state of the tower and all related properties
     */
    void update() override;

    /**
     * Performs late updates for the tower
     */
    void lateUpdate() override;
};

} // namespace state
