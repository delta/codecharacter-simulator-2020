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

void BotTransformState::enter() { bot->constructTower(); }

std::unique_ptr<IActorState> BotTransformState::update() const {
    if (bot->getHp() == 0)
        return std::make_unique<BotDeadState>(bot);
    return nullptr;
}

void BotTransformState::exit() { bot->setTransforming(false); }

} // namespace state
