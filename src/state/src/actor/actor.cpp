/**
 * @file actor.cpp
 * @brief Defintion of Actor class.
 *
 */

#include "state/actor/actor.h"
#include "physics/vector.hpp"

namespace state {

Actor::Actor(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
             DoubleVec2D position)
    : id(id), player_id(player_id), actor_type(actor_type), hp(hp),
      position(position){};

ActorId Actor::getActorId() { return id; }
void Actor::setActorIdIncrement(ActorId actor_id) {
  if (actor_id < 0) {
    throw std::out_of_range("`actor_id` cannot be negative");
  }
  actor_id_increment = actor_id;
}

ActorId Actor::getNextActorId() { return actor_id_increment++; }

PlayerId Actor::getPlayerId() const { return player_id; }

ActorType Actor::getActorType() const { return actor_type; }

size_t Actor::getHp() const { return hp; }

void Actor::setHp(size_t hp) {
  if (hp < 0) {
    throw std::out_of_range("`hp` must be a positive value");
  }
  if (hp > this->max_hp) {
    throw std::out_of_range("`hp` cannot be greater than max_hp");
  }
  this->hp = hp;
}

size_t Actor::getMaxHp() { return max_hp; }

size_t Actor::getLatestHp() const {
  return std::min<size_t>(0, hp - damage_incurred);
}

void Actor::damage(size_t damage_amout) {
  damage_incurred = std::min<size_t>(hp, damage_incurred + damage_amout);
}

DoubleVec2D Actor::getPosition() const { return position; }

size_t Actor::getAge() const { return age; }

} // namespace state
