/**
 * @file bot.h
 * @brief Declares the Bot Class
 */

#pragma once

#include "state/actor/blaster.h"
#include "state/actor/bot_states/bot_state.h"
#include "state/actor/unit.h"

namespace state {

class STATE_EXPORT Bot : public Unit, public Blaster {
  private:
    /**
     * @brief Controls logic for bot's current state
     */
    std::unique_ptr<BotState> state;

    /**
     * @brief The destination after which the bot transitions to blast state
     */
    DoubleVec2D final_destination;

    /**
     * @brief Tracks final_destination
     */
    bool is_final_destination_set;

    /**
     * @brief
     */
    DoubleVec2D transform_destination;

    /**
     * @brief
     */
    bool is_transform_destinaion_set;

    /**
     * @brief
     */
    bool transforming;

  public:
    /**
     * @brief Construct a new Bot object
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
     * @brief Construct a new Bot object, with auto incrementing id.
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
     * @brief
     *
     * @return true
     * @return false
     */
    bool isFinalDestinationSet() const;

    /**
     * @brief Set the Final Destination object
     *
     * @param final_destination
     */
    void setFinalDestination(DoubleVec2D final_destination);

    /**
     * @brief Get the Final Destination object
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getFinalDestination() const;

    /**
     * @brief
     */
    void clearFinalDestination();

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool isTransformDestinationSet() const;

    /**
     * @brief Set the Transform Destination object
     *
     * @param transform_destination
     */
    void setTransformDestination(DoubleVec2D transform_destination);

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool isTransforming() const;

    /**
     * @brief Set the Transforming object
     *
     * @param p_transforming
     */
    void setTransforming(bool p_transforming);

    /**
     * @brief Get the Transform Destination object
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getTransformDestination() const;

    /**
     * @brief
     */
    void clearTransformDestination();

    /**
     * @brief Get the State object
     *
     * @return BotStateName
     */
    BotStateName getState() const;

    /**
     * @see Blaster#Blast
     */
    void blast() override;

    /**
     * @brief Changes bot state to BotStateName::Transform and the state
     * transitions to BotStateName::IDLE
     *
     */
    void transform();

    /**
     * @brief Updates the state of the bot and all related properties
     *
     */
    void update() override;

    /**
     * Performs late updates for the bot
     */
    void lateUpdate() override;
};

} // namespace state
