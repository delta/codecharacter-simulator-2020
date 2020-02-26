/**
 * @file actor.h
 *  Declares the Base Actor class that all actors derive from.
 */

#pragma once

#include "physics/vector.hpp"
#include "state/interfaces/i_updatable.h"
#include "state/score_manager/score_manager.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {
/**
 *  Actor base class
 */
class STATE_EXPORT Actor : public IUpdatable {
  protected:
    /**
     *  Unique ID of actor
     */
    const ActorId id;

    /**
     *  Static counter which assigns unique incrementing ID to each actor
     *  instance
     */
    static ActorId next_actor_id;

    /**
     *  ID of player that Actor belongs to
     */
    const PlayerId player_id;

    /**
     *  Type of the Actor
     */
    ActorType actor_type;

    /**
     *  Current HitPoints of the Actor.
     */
    size_t hp;

    /**
     *  Maximum possible HitPoints of Actor.
     *
     */
    size_t max_hp;

    /**
     *  Amount of damage the soldier incurred in the current turn
     *  Applied to hp at the end of the turn
     */
    size_t damage_incurred;

    /**
     *  Actor position
     */
    DoubleVec2D position;

    /**
     * Score manager to update scores
     */
    ScoreManager *score_manager;

  public:
    /**
     *  Construct a new Actor object
     *
     * @param id
     * @param player_id
     * @param actor_type
     * @param hp
     * @param max_hp
     * @param position
     */
    Actor(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
          size_t max_hp, DoubleVec2D position, ScoreManager *score_manager);

    /**
     *  Construct a new Actor object, with auto incrementing id.
     *
     * @param player_id
     * @param actor_type
     * @param hp
     * @param max_hp
     * @param position
     */
    Actor(PlayerId player_id, ActorType actor_type, size_t hp, size_t max_hp,
          DoubleVec2D position, ScoreManager *score_manager);

    /**
     * Destructor
     */
    virtual ~Actor() = default;

    /**
     *  Gets the new ActorId to assign to new actor.
     *
     * @return The ActorId to assign
     */
    static ActorId getNextActorId();

    /**
     *  Get the ActorId of Actor.
     *
     * @return ActorId
     */
    ActorId getActorId() const;

    /**
     *  Get the PlayerId of Actor.
     *
     * @return PlayerId
     */
    PlayerId getPlayerId() const;

    /**
     *  Get the Actor type of Actor.
     *
     * @return ActorType
     */
    ActorType getActorType() const;

    /**
     *  Get the current Hp of Actor.
     *
     * @return size_t
     */
    size_t getHp() const;

    /**
     *  Set the Hp of Actor
     *
     * @throw std::out_of_range If hp is negative, or greater than max_hp.
     */
    void setHp(size_t hp);

    /**
     *  Get the Maximum Hp of Actor
     *
     * @return size_t
     */
    size_t getMaxHp() const;

    /**
     *  Gets the latest Hp of Actor after every turn.
     *  All damages occuring to actor is accumulated for every turn.
     *  At the end of the turn this accumulated damage is applied. This method
     *  returns hp at the end of the turn.
     *
     * @return size_t
     */
    size_t getLatestHp() const;

    /**
     *  Gets the damage incurred by the Actor during this turn.
     *  All damages occuring to actor is accumulated
     *
     * @return size_t
     */
    size_t getDamageIncurred() const;

    /**
     *  Set the damage incurred of the Actor
     *
     * @throw std::out_of_range If the damage incurred is negative
     */
    void setDamageIncurred(int64_t damage_incurred);

    /**
     *  Used to reduce hp of the Actor.
     *  Increases damage_incurred of Actor upto its Hp.
     *
     * @param damage_amount
     */
    void damage(size_t damage_amount);

    /**
     *  Get the Position of Actor
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getPosition() const;

    /**
     * Set the Position of Actor
     *
     */
    void setPosition(DoubleVec2D p_position);

    /**
     * Returns a pointer to the score manager
     *
     * @return ScoreManager*
     */
    ScoreManager *getScoreManager() const;
};

} // namespace state
