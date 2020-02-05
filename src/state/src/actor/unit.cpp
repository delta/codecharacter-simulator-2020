/**
 * @file unit.cpp
 * Definition of Unit class
 */

#include "state/actor/unit.h"
#include "physics/vector.hpp"

namespace state {

Unit::Unit(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
           size_t max_hp, size_t speed, DoubleVec2D position)
    : Actor(id, player_id, actor_type, hp, max_hp, position), speed(speed),
      destination(DoubleVec2D::null), is_destination_set(false),
      new_position(DoubleVec2D::null), is_new_position_set(false) {}

Unit::Unit(PlayerId player_id, ActorType actor_type, size_t hp, size_t max_hp,
           size_t speed, DoubleVec2D position)
    : Actor(id, player_id, actor_type, hp, max_hp, position), speed(speed),
      destination(DoubleVec2D::null), is_destination_set(false),
      new_position(DoubleVec2D::null), is_new_position_set(false) {}

size_t Unit::getSpeed() const { return speed; }

DoubleVec2D Unit::getDestination() const { return destination; }

void Unit::setDestination(DoubleVec2D new_destination) {
    clearDestination();
    destination = new_destination;
    is_destination_set = true;
}

bool Unit::isDestinationSet() const { return is_destination_set; }

void Unit::clearDestination() {
    destination = DoubleVec2D::null;
    is_destination_set = false;
}

DoubleVec2D Unit::getNewPosition() const { return new_position; }

void Unit::setNewPosition(DoubleVec2D next_position) {
    new_position = next_position;
    is_new_position_set = true;
}

void Unit::move(DoubleVec2D destination) { setDestination(destination); }

} // namespace state
