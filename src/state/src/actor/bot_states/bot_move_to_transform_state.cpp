/**
 * @file bot_move_blast_state.cpp
 * Defines the bot Move_Transform class
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

    // when transform_destination is not set
    if (!bot->isTransformDestinationSet()) {
        // when destination is set
        if (bot->isDestinationSet())
            return make_unique<BotMoveState>(bot);

        // when final_destination is set
        if (bot->isTransformDestinationSet())
            return make_unique<BotMoveToTransformState>(bot);

        // when is_blasting is set to true
        if (bot->isBlasting())
            return make_unique<BotBlastState>(bot);

        // when is_transforming is set to true
        if (bot->isTransforming())
            return make_unique<BotTransformState>(bot);

        // no other action done
        return make_unique<BotIdleState>(bot);
    }

    // transfrom_destination is set

    // when bot is ready to blast, no more distance left to move
    if (bot->getPosition() == bot->getTransformDestination())
        return make_unique<BotTransformState>(bot);

    // TODO: Get path planner and set new postiion

    return nullptr;
}

void BotMoveToTransformState::exit() {}
} // namespace state
