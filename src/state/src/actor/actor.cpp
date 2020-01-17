/**
 * @file actor.cpp
 * @brief Defintion of Actor class.
 *
 */

#include "state/actor/actor.h"
#include "physics/vector.hpp"

namespace state {

ActorId Actor::next_actor_id = 0;

Actor::Actor(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
             size_t max_hp, Vec2D position)
    : id(id), player_id(player_id), actor_type(actor_type), hp(hp),
      max_hp(max_hp), position(position){};

Actor::Actor(PlayerId player_id, ActorType actor_type, size_t hp, size_t max_hp,
             Vec2D position)
    : id(Actor::getNextActorId()), player_id(player_id), actor_type(actor_type),
      hp(hp), position(position){};

ActorId Actor::getActorId() const { return id; }

ActorId Actor::getNextActorId() { return next_actor_id++; }

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

size_t Actor::getMaxHp() const { return max_hp; }

size_t Actor::getLatestHp() const {
  return std::min<size_t>(0, hp - damage_incurred);
}

void Actor::damage(size_t damage_amout) {
  damage_incurred = std::min<size_t>(hp, damage_incurred + damage_amout);
}

Vec2D Actor::getPosition() const { return position; }

} // namespace state
