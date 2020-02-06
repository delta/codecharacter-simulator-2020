/**
 * @file bot_move_blast_state.cpp
 * Defines the bot Move_Blast class
 */

#include "state/actor/bot_states/bot_move_to_blast_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_blast_state.h"
#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot_states/bot_idle_state.h"
#include "state/actor/bot_states/bot_move_state.h"
#include "state/actor/bot_states/bot_move_to_transform_state.h"
#include "state/actor/bot_states/bot_transform_state.h"

namespace state {
BotMoveToBlastState::BotMoveToBlastState(Bot *bot)
    : BotState(BotStateName::MOVE_TO_BLAST, bot) {}
void BotMoveToBlastState::enter() {}

std::unique_ptr<IActorState> BotMoveToBlastState::update() const {
    using std::make_unique;

    // check if bot is dead
    if (bot->getHp() == 0)
        return make_unique<BotDeadState>(bot);

    if (!bot->isFinalDestinationSet()) {
        // state transition based on characteristics
        if (bot->isDestinationSet())
            return make_unique<BotMoveState>(bot);

        if (bot->isTransformDestinationSet())
            return make_unique<BotMoveToTransformState>(bot);

        if (bot->isBlasting())
            return make_unique<BotBlastState>(bot);

        if (bot->isTransforming())
            return make_unique<BotTransformState>(bot);

        // no other action done
        return make_unique<BotIdleState>(bot);
    }

    // bot ready to blast
    if (bot->getPosition() == bot->getFinalDestination())
        return make_unique<BotBlastState>(bot);

    // TODO: use path planner and validate next position
    // TODO: set new position or transition to ilde state

    return nullptr;
}

void BotMoveToBlastState::exit() { bot->clearFinalDestination(); }
} // namespace state
