/**
 * @file actor.h
 * @brief Declares the Base Actor class that all actors derive from.
 */

#pragma once

#include "physics/vector.hpp"
#include "state/interfaces/i_updatable.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {
/**
 * @brief Actor base class
 */
class STATE_EXPORT Actor : public IUpdatable {
protected:
  /**
   * @brief Unique ID of actor
   */
  const ActorId id;

  /**
   * @brief Static counter which assigns unique incrementing ID to each actor
   * instance
   */
  static ActorId next_actor_id;

  /**
   * @brief ID of player that Actor belongs to
   */
  const PlayerId player_id;

  /**
   * @brief Type of the Actor
   */
  ActorType actor_type;

  /**
   * @brief Current HitPoints of the Actor.
   */
  size_t hp;

  /**
   * @brief Maximum possible HitPoints of Actor.
   *
   */
  // TODO: set max_hp from constants here.
  static const size_t max_hp = 50;

  /**
   * @brief Amount of damage the soldier incurred in the current turn
   *        Applied to hp at the end of the turn
   */
  size_t damage_incurred;

  /**
   * @brief Actor position
   */
  Vec2D position;

public:
  Actor(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
        Vec2D position);

  Actor(PlayerId player_id, ActorType actor_type, size_t hp, Vec2D position);

  virtual ~Actor() {}

  /**
   * @brief Gets the new ActorId to assign to new actor.
   *
   * @return The ActorId to assign
   */
  static ActorId getNextActorId();

  /**
   * @brief Get the ActorId of Actor.
   *
   * @return ActorId
   */
  ActorId getActorId() const;

  /**
   * @brief Get the PlayerId of Actor.
   *
   * @return PlayerId
   */
  PlayerId getPlayerId() const;

  /**
   * @brief Get the Actorype of Actor.
   *
   * @return ActorType
   */
  ActorType getActorType() const;

  /**
   * @brief Get the current Hp of Actor.
   *
   * @return size_t
   */
  size_t getHp() const;

  /**
   * @brief Set the Hp of Actor
   *
   * @throw std::out_of_range If hp is negative, or greater than max_hp.
   */
  void setHp(size_t hp);

  /**
   * @brief Get the Maximum Hp of Actor
   *
   * @return size_t
   */
  static size_t getMaxHp();

  /**
   * @brief Gets the latest Hp of Actor after every turn.
   *        All damages occuring to actor is accumulated for every turn. At the
   * end of the turn this accumulated damage is applied. This method returns hp
   * at the end of the turn.
   *
   * @return size_t
   */
  size_t getLatestHp() const;

  /**
   * @brief Used to reduce hp of the Actor.
   *        Increases damage_incurred of Actor upto its Hp.
   *
   * @param damage_amout
   */
  void damage(size_t damage_amout);

  /**
   * @brief Get the Position of Actor
   *
   * @return Vec2D
   */
  Vec2D getPosition() const;
};

} // namespace state
