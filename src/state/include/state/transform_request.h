/**
 * @file transform_request.h
 * Declares a struct which represents all the transform requests in one turn
 */

#include "state/utilities.h"

namespace state {
struct TransformRequest {
    /**
     * Id of the bot requesting to transform
     */
    ActorId bot_id;

    /**
     * Tower position where the bot is trying to transform
     */
    DoubleVec2D position;
};
} // namespace state
