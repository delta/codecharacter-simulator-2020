/**
 * @file unit.h
 * @brief Declares the Unit class that all movable units actors derive from.
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
 * @brief Unit base class
 *
 */
class STATE_EXPORT Unit : public Actor {
  protected:
    /**
     * @brief Unit speed in map-units per turn.
     */
    size_t speed;
    /**
     * @brief Destination that the soldier needs to navigate to. Valid only if
     * is_destination_set is true
     */
    Vec2D destination;
    /**
     * @brief true -> if unit is moving to set destination
     *        false -> otherwise
     */
    bool is_destination_set;
    /**
     * @brief  Position Unit should move to if Unit is moving. Used in updating
     * Unit position.
     */
    Vec2D new_position;
    /**
     * @brief true -> if new_position is set
     *        false -> otherwise
     */
    bool is_new_position_set;

  public:
    /**
     * @brief Construct a new Unit object
     *
     * @param id
     * @param player_id
     * @param actor_type
     * @param hp
     * @param position
     */
    Unit(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
         size_t max_hp, size_t speed, Vec2D position);

    /**
     * @brief Get the Unit's speed.
     *
     * @return size_t
     */
    size_t getSpeed() const;

    /**
     * @brief Get the Destination of Unit
     *
     * @return Vec2D
     */
    Vec2D getDestination() const;

    /**
     * @brief Set the Destination of Unit
     *
     * @param destination
     */
    void setDestination(Vec2D destination);

    /**
     * @brief check if destination is set
     *
     * @return true
     * @return false
     */
    bool isDestinationSet() const;

    /**
     * @brief Resets current destination and sets is_destination_set to false.
     */
    void clearDestination();

    /**
     * @brief Get the new_position
     *
     * @return Vec2D new_position of Unit
     */
    Vec2D getNewPosition() const;

    /**
     * @brief Set the new_position of Unit
     *
     * @param position The position to set new_position to.
     */
    void setNewPosition(Vec2D position);

    /**
     * @brief Check if new_position is set
     *
     * @return true
     * @return false
     */
    bool isNewPostitionSet() const;

    /**
     * @brief Resets new_position and sets is_new_position_set to false.
     */
    void clearNewPosition();

    /**
     * @brief Method to give the move command to this soldier
     *
     * @param destination The destination to move to.
     */
    void move(Vec2D destination);

    /**
     * @brief Update function of Unit
     */
    virtual void update() = 0;

    /**
     * @brief Late Update function of unit. After the end of every turn.
     */
    virtual void lateUpdate() = 0;
};

} // namespace state
