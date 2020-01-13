/**
 * @file unit.cpp
 * @brief Definition of Unit class
 *
 */

#include "state/actor/unit.h"
#include "physics/vector.hpp"
#include "state/actor/actor.h"

namespace state {

Unit::Unit(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
           DoubleVec2D position)
    : Actor(id, player_id, actor_type, hp, position), destination(Vec2D::null),
      is_destination_set(false), new_position(DoubleVec2D::null),
      is_new_position_set(false) {}

size_t Unit::getSpeed() const { return speed; }

Vec2D Unit::getDestination() const { return destination; }

void Unit::setDestination(Vec2D destination) {
  this->destination = destination;
  is_destination_set = true;
}

bool Unit::isDestinationSet() const { return is_destination_set; }

void Unit::clearDestination() {
  destination = Vec2D::null;
  is_destination_set = false;
}

DoubleVec2D Unit::getNewPosition() const { return new_position; }

void Unit::setNewPosition(DoubleVec2D position) {
  this->new_position = position;
  is_new_position_set = true;
}

bool Unit::isNewPostitionSet() const { return is_new_position_set; }

void Unit::clearNewPosition() {
  new_position = DoubleVec2D::null;
  is_new_position_set = false;
}

void Unit::move(Vec2D destination) {
  this->destination = destination;
  this->is_destination_set = true;
}

} // namespace state
