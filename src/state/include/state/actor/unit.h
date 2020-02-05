/**
 * @file unit.h
 *  Declares the Unit class that all movable units actors derive from.
 *
 */

#pragma once

#include "physics/vector.hpp"
#include "state/actor/actor.h"
#include "state/interfaces/i_updatable.h"
#include "state/state_export.h"
#include "state/utilities.h"

namespace state {
/**
 *  Unit base class
 *
 */
class STATE_EXPORT Unit : public Actor {
  protected:
    /**
     *  Unit speed in map-units per turn.
     */
    size_t speed;
    /**
     *  Destination that the soldier needs to navigate to. Valid only if
     * is_destination_set is true
     */
    DoubleVec2D destination;
    /**
     *  true -> if unit is moving to set destination
     *        false -> otherwise
     */
    bool is_destination_set;
    /**
     *   Position Unit should move to if Unit is moving. Used in updating
     * Unit position.
     */
    DoubleVec2D new_position;
    /**
     *  true -> if new_position is set
     *        false -> otherwise
     */
    bool is_new_position_set;

  public:
    /**
     *  Construct a new Unit object
     *
     * @param id
     * @param player_id
     * @param actor_type
     * @param hp
     * @param position
     */
    Unit(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
         size_t max_hp, size_t speed, DoubleVec2D position);

    /**
     *  Construct a new Unit object, with auto incrementing id.
     *
     * @param player_id
     * @param actor_type
     * @param hp
     * @param position
     */
    Unit(PlayerId player_id, ActorType actor_type, size_t hp, size_t max_hp,
         size_t speed, DoubleVec2D position);

    /**
     *  Get the Unit's speed.
     *
     * @return size_t
     */
    size_t getSpeed() const;

    /**
     *  Get the Destination of Unit
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getDestination() const;

    /**
     *  Set the Destination of Unit
     *
     * @param destination
     */
    void setDestination(DoubleVec2D destination);

    /**
     *  check if destination is set
     *
     * @return true
     * @return false
     */
    bool isDestinationSet() const;

    /**
     *  Resets current destination and sets is_destination_set to false.
     */
    void clearDestination();

    /**
     *  Get the new_position
     *
     * @return DoubleVec2D new_position of Unit
     */
    DoubleVec2D getNewPosition() const;

    /**
     *  Set the new_position of Unit
     *
     * @param position The position to set new_position to.
     */
    void setNewPosition(DoubleVec2D position);

    /**
     *  Check if new_position is set
     *
     * @return true
     * @return false
     */
    bool isNewPostitionSet() const;

    /**
     *  Resets new_position and sets is_new_position_set to false.
     */
    void clearNewPosition();

    /**
     *  Method to give the move command to this soldier
     *
     * @param destination The destination to move to.
     */
    void move(DoubleVec2D destination);

    /**
     *  Update function of Unit
     */
    virtual void update() = 0;

    /**
     *  Late Update function of unit. After the end of every turn.
     */
    virtual void lateUpdate() = 0;
};

} // namespace state
