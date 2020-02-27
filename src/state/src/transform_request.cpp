#include "state/transform_request.h"

namespace state {
TransformRequest::TransformRequest(PlayerId player_id, ActorId bot_id,
                                   DoubleVec2D position)
    : player_id(player_id), bot_id(bot_id), position(position) {}

ActorId TransformRequest::getBotId() { return bot_id; }

DoubleVec2D TransformRequest::getPosition() { return position; }

PlayerId TransformRequest::getPlayerId() { return player_id; }
} // namespace state
