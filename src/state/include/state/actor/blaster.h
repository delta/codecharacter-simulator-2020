/**
 * @file blaster.h
 *  A class to expose blasting properties
 */

#pragma once

#include "state/actor/actor.h"

#include <vector>

namespace state {

class STATE_EXPORT Blaster : public Actor {
protected:
  size_t blast_range;
  size_t damage_points;

public:
  /**
   * Constructors
   */
  Blaster(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
          size_t max_hp, size_t damage_incurred, Vec2D pos);

  Blaster(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
          size_t max_hp, size_t damage_incurred, Vec2D pos, int blast_range,
          int damage_points);

  /**
   * The actor blasts and the actor's state transitions to dead state
   */
  virtual void Blast() = 0;
};

} // namespace state
