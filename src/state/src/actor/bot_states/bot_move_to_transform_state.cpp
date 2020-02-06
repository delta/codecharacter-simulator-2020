/**
 * @file bot_move_to_transform_state.cpp
 * Defines the bot Move to Transform class
 */

#include "state/actor/bot_states/bot_move_to_transform_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_blast_state.h"
#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot_states/bot_idle_state.h"
#include "state/actor/bot_states/bot_move_state.h"
#include "state/actor/bot_states/bot_move_to_blast_state.h"
#include "state/actor/bot_states/bot_state.h"
#include "state/actor/bot_states/bot_transform_state.h"

namespace state {
BotMoveToTransformState::BotMoveToTransformState(Bot *bot)
    : BotState(BotStateName::MOVE_TO_TRANSFORM, bot) {}
void BotMoveToTransformState::enter() {}

std::unique_ptr<IActorState> BotMoveToTransformState::update() const {
    using std::make_unique;

    // check if bot is dead
    if (bot->getHp() == 0)
        return make_unique<BotDeadState>(bot);

    if (!bot->isTransformDestinationSet()) {
        // state transitions based on characteristics
        if (bot->isDestinationSet())
            return make_unique<BotMoveState>(bot);

        if (bot->isFinalDestinationSet())
            return make_unique<BotMoveToBlastState>(bot);

        if (bot->isBlasting())
            return make_unique<BotBlastState>(bot);

        if (bot->isTransforming())
            return make_unique<BotTransformState>(bot);

        return make_unique<BotIdleState>(bot);
    }

    // bot ready to transform
    if (bot->getPosition() == bot->getTransformDestination())
        return make_unique<BotTransformState>(bot);

    // TODO: use path planner and validate next position
    // TODO: set new position or transition to ilde state

    return nullptr;
}

void BotMoveToTransformState::exit() { bot->clearTransformDestination(); }
} // namespace state
