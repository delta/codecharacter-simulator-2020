/**
 * @file blaster.cpp
 * Defines the blaster class
 */

#include "state/actor/blaster.h"

namespace state {
Blaster::Blaster(ActorId id, PlayerId player_id, ActorType actor_type,
                 size_t hp, size_t max_hp, size_t damage_incurred, Vec2D pos)
    : Actor(id, player_id, actor_type, hp, max_hp, pos) {}

Blaster::Blaster(ActorId id, PlayerId player_id, ActorType actor_type,
                 size_t hp, size_t max_hp, size_t damage_incurred, Vec2D pos,
                 int blast_range, int damage_points)
    : Actor(id, player_id, actor_type, hp, max_hp, pos),
      blast_range(blast_range), damage_points(damage_points) {}
} // namespace state
