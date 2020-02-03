/**
 * @file bot.h
 *  Declares the Bot Class
 */

#pragma once

#include "state/actor/blaster.h"
#include "state/actor/bot_states/bot_state.h"
#include "state/actor/unit.h"

namespace state {

class STATE_EXPORT Bot : public Unit, public Blaster {
  private:
    /**
     *  Controls logic for bot's current state
     */
    std::unique_ptr<BotState> state;

    /**
     *  The destination after which the bot transitions to blast state
     */
    DoubleVec2D final_destination;

    /**
     *  Tracks final_destination
     */
    bool is_final_destination_set;

    /**
     *  The destination after which the bot becomes a tower
     */
    DoubleVec2D transform_destination;

    /**
     *  Tracks transform_destination
     */
    bool is_transform_destination_set;

    /**
     *  True if the bot is going to transform
     *  False if otherwise
     */
    bool is_transforming;

  public:
    /**
     *  Construct a new Bot object
     *
     * @param id
     * @param player_id
     * @param actor_type
     * @param hp
     * @param max_hp
     * @param position
     * @param speed
     * @param blast_range
     * @param damage_points
     * @param blast_callback
     */
    Bot(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
        size_t max_hp, DoubleVec2D position, size_t speed, size_t blast_range,
        size_t damage_points, BlastCallback blast_callback);

    /**
     *  Construct a new Bot object, with auto incrementing id.
     *
     * @param player_id
     * @param actor_type
     * @param hp
     * @param max_hp
     * @param position
     * @param speed
     * @param blast_range
     * @param damage_points
     * @param blast_callback
     */
    Bot(PlayerId player_id, ActorType actor_type, size_t hp, size_t max_hp,
        DoubleVec2D position, size_t speed, size_t blast_range,
        size_t damage_points, BlastCallback blast_callback);

    /**
     *  check if final_destination is set
     *
     * @return true
     * @return false
     */
    bool isFinalDestinationSet() const;

    /**
     *  Sets final_destination with passed destination
     *
     * @param final_destination
     */
    void setFinalDestination(DoubleVec2D p_final_destination);

    /**
     *  Get the value of final_destination
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getFinalDestination() const;

    /**
     *  resets final_destination to DoubleVec2D::null
     */
    void clearFinalDestination();

    /**
     *  check if transform_destination is set
     *
     * @return true
     * @return false
     */
    bool isTransformDestinationSet() const;

    /**
     *  Set the transform_destination property
     *
     * @param transform_destination
     */
    void setTransformDestination(DoubleVec2D p_transform_destination);

    /**
     *  checks if is_transforming property is set or not
     *
     * @return true
     * @return false
     */
    bool isTransforming() const;

    /**
     *  Set the is_transforming property
     *
     * @param p_transforming
     */
    void setTransforming(bool p_transforming);

    /**
     *  Get the transform_destination property
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getTransformDestination() const;

    /**
     *  resets transform_destination
     */
    void clearTransformDestination();

    /**
     *  Get the current state of bot
     *
     * @return BotStateName
     */
    BotStateName getState() const;

    /**
     * @see Blaster#Blast
     */
    void blast() override;

    /**
     *  Changes bot state to Transform
     */
    void transform();

    /**
     *  Updates the state of the bot and all related properties
     *
    void update() override;
     */
    void lateUpdate() override;
};

} // namespace state
