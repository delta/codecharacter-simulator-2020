/**
 * @file bot_move_state.cpp
 * @brief Definition of bot move state
 */

#include "state/actor/bot_states/bot_transform_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

BotTransformState::BotTransformState(Bot *bot)
    : BotState(BotStateName::TRANSFORM, bot) {}

void BotTransformState::enter() {}

std::unique_ptr<IActorState> BotTransformState::update() {

    if (bot->getHp() == 0) {
        return std::make_unique<BotDeadState>(bot);
    }
    //
}

void BotTransformState::exit() {}

} // namespace state
