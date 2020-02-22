/**
 * @file actor.cpp
 *  Defintion of Actor class.
 */

#include "state/actor/actor.h"
#include "physics/vector.hpp"

namespace state {

ActorId Actor::next_actor_id = 0;

Actor::Actor(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
             size_t max_hp, DoubleVec2D position)
    : id(id), player_id(player_id), actor_type(actor_type), hp(hp),
      max_hp(max_hp), damage_incurred(0), position(position) {}

Actor::Actor(PlayerId player_id, ActorType actor_type, size_t hp, size_t max_hp,
             DoubleVec2D position)
    : id(Actor::getNextActorId()), player_id(player_id), actor_type(actor_type),
      hp(hp), max_hp(max_hp), damage_incurred(0), position(position) {}

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

void Actor::setDamageIncurred(int64_t p_damage_incurred) {
    if (p_damage_incurred < 0) {
        throw std::out_of_range("`damage_incurred` must be a positive value");
    }
    damage_incurred = std::min(p_damage_incurred, (int64_t) hp);
}

size_t Actor::getDamageIncurred() const { return damage_incurred; }

size_t Actor::getMaxHp() const { return max_hp; }

size_t Actor::getLatestHp() const {
    return std::max<size_t>(0, hp - damage_incurred);
}

void Actor::damage(size_t damage_amout) {
    damage_incurred = std::min<size_t>(hp, damage_incurred + damage_amout);
}

DoubleVec2D Actor::getPosition() const { return position; }

void Actor::setPosition(DoubleVec2D p_position) {
    if (!p_position) {
        throw std::domain_error("Position not inside the range of map");
    }
    position = p_position;
}

} // namespace state
