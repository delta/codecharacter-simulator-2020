/**
 * @file bot.cpp
 *  Definition of Bot class
 */

#include "state/actor/bot.h"
#include "state/actor/blaster.h"
#include "state/actor/bot_states/bot_idle_state.h"

namespace state {

Bot::Bot(ActorId id, PlayerId player_id, ActorType actor_type, size_t hp,
         size_t max_hp, DoubleVec2D position, size_t speed, size_t blast_range,
         size_t damage_pointsm, BlastCallback blast_callback)
    : Unit::Unit(id, player_id, actor_type, hp, max_hp, speed, position),
      Blaster::Blaster(blast_range, damage_points, blast_callback),
      state(std::make_unique<BotIdleState>(this)) {}

void Bot::clearFinalDestination() {
    final_destination = DoubleVec2D::null;
    is_final_destination_set = false;
}

void Bot::clearTransformDestination() {
    transform_destination = DoubleVec2D::null;
    is_transform_destination_set = false;
}

bool Bot::isFinalDestinationSet() const { return is_final_destination_set; }

void Bot::setFinalDestination(DoubleVec2D p_final_destination) {
    clearDestination();
    clearFinalDestination();
    final_destination = p_final_destination;
    is_final_destination_set = true;
}

bool Bot::isTransformDestinationSet() const {
    return is_transform_destination_set;
}

void Bot::setTransformDestination(DoubleVec2D p_transform_destination) {
    clearDestination();
    clearFinalDestination();
    transform_destination = p_transform_destination;
    is_transform_destination_set = true;
}

void Bot::setTransforming(bool p_transforming) {
    transforming = p_transforming;
}

BotStateName Bot::getState() const { return state->getName(); }

void Bot::blast() {
    clearDestination();
    clearFinalDestination();
    clearTransformDestination();
    setBlasting(true);
}

void Bot::transform() {
    clearDestination();
    clearFinalDestination();
    clearTransformDestination();
    setTransforming(true);
}

void Bot::lateUpdate() {
    // Updating the hp of the Bot
    size_t latest_hp = getLatestHp();
    setHp(latest_hp);

    // Resetting the damage incurred
    setDamageIncurred(0);

    // get new state based on bot properties
    auto new_state = state->update();

    // until no state transitions occur, in a single frame
    while (new_state != nullptr) {
        state->exit();
        /*
            Here, state.reset destroys the state it is currently managing and
           starts managing the new state object passed to it
        */
        state.reset(static_cast<BotState *>(new_state.release()));
        state->enter();
        new_state = state->update();
    }
}

void Bot::update() {
    // get new state based on bot properties
    auto new_state = state->update();

    // until no state transitions occur, in a single frame
    while (new_state != nullptr) {
        // state transition occured
        state->exit();
        /*
            Here, state.reset destroys the state it is currently managing and
           starts managing the new state object passed to it
        */
        state.reset(static_cast<BotState *>(new_state.release()));
        state->enter();
        new_state = state->update();
    }
}

} // namespace state
