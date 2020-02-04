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
     * @brief The destination after which the bot becomes a wall
     */
    DoubleVec2D transform_destination;

    /**
     * @brief Tracks transform_destination
     */
    bool is_transform_destination_set;

    /**
     * @brief  True if the bot is going to transform
     *         False if otherwise
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
     * @brief check if final_destination is set
     *
     * @return true
     * @return false
     */
    bool isFinalDestinationSet() const;

    /**
     * @brief Sets final_destination with passed destination
     *
     * @param final_destination
     */
    void setFinalDestination(DoubleVec2D p_final_destination);

    /**
     * @brief Get the value of final_destination
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getFinalDestination() const;

    /**
     * @brief resets final_destination to DoubleVec2D::null
     */
    void clearFinalDestination();

    /**
     * @brief check if transform_destination is set
     *
     * @return true
     * @return false
     */
    bool isTransformDestinationSet() const;

    /**
     * @brief Set the transform_destination property
     *
     * @param transform_destination
     */
    void setTransformDestination(DoubleVec2D p_transform_destination);

    /**
     * @brief checks if transforming property is set or not
     *
     * @return true
     * @return false
     */
    bool isTransforming() const;

    /**
     * @brief Set the transforming property
     *
     * @param p_transforming
     */
    void setTransforming(bool p_transforming);

    /**
     * @brief Get the transform_destination property
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getTransformDestination() const;

    /**
     * @brief resets transform_destination
     */
    void clearTransformDestination();

    /**
     * @brief Get the current state of bot
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
