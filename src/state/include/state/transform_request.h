/**
 * @file transform_request.h
 * Declares a struct which represents all the transform requests in one turn
 */

#pragma once
#include "physics/vector.hpp"
#include "state/actor/actor.h"
#include "state/utilities.h"

#include <functional>

namespace state {

class TransformRequest {
    /**
     * Player Id of player bot requesting to transform
     */
    PlayerId player_id;

    /**
     * Id of the bot requesting to transform
     */
    ActorId bot_id;

    /**
     * Position where the bot is trying to transform
     */
    DoubleVec2D position;

  public:
    /**
     * Constructor
     */
    TransformRequest(PlayerId player_id, ActorId bot_id, DoubleVec2D position);
    /**
     * Returns the actor id of the bot requesting to transform
     *
     * @return ActorId Bot_id
     */
    ActorId getBotId();

    /**
     * Returns the Position where the bot is requesting to transform
     *
     * @return DoubleVec2D
     */
    DoubleVec2D getPosition();

    /**
     * Returns the Player Id object
     *
     * @return PlayerId
     */
    PlayerId getPlayerId();
};
} // namespace state
