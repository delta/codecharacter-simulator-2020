/**
 * @file bot_move_state.cpp
 * Definition of bot move state
 */

#include "state/actor/bot_states/bot_transform_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

BotTransformState::BotTransformState(Bot *bot)
    : BotState(BotStateName::TRANSFORM, bot) {}

void BotTransformState::enter() {
    // send build request to create tower and kill this bot in State
    bot->constructTower();
    bot->setTransforming(true);
}

std::unique_ptr<IActorState> BotTransformState::update() const {
    return std::make_unique<BotDeadState>(bot);
}

void BotTransformState::exit() { bot->setTransforming(false); }

} // namespace state
