/**
 * @file bot_idle_state.cpp
 *  Definition of bot idle state
 */

#include "state/actor/bot_states/bot_idle_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_blast_state.h"
#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot_states/bot_move_state.h"
#include "state/actor/bot_states/bot_move_to_blast_state.h"
#include "state/actor/bot_states/bot_move_to_transform_state.h"
#include "state/actor/bot_states/bot_state.h"
#include "state/actor/bot_states/bot_transform_state.h"

namespace state {

BotIdleState::BotIdleState(Bot *bot) : BotState(BotStateName::IDLE, bot) {}

void BotIdleState::enter() {}

std::unique_ptr<IActorState> BotIdleState::update() const {
    using std::make_unique;
    if (bot->getHp() == 0)
        return make_unique<BotDeadState>(bot);
    if (bot->getDestination())
        return make_unique<BotMoveState>(bot);
    if (bot->getFinalDestination())
        return make_unique<BotMoveToBlastState>(bot);
    if (bot->getTransformDestination())
        return make_unique<BotMoveToTransformState>(bot);
    if (bot->isBlasting())
        return make_unique<BotBlastState>(bot);
    if (bot->isTransforming())
        return make_unique<BotTransformState>(bot);

    return nullptr;
}

void BotIdleState::exit() {}

} // namespace state
