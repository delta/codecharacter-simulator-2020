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
    // do nothing
    // if alive in late update and isTransforming set to true, call the
    // construct tower callback
    // bot->setTransforming(true);
    // bot->constructTower(bot->getPlayerId(), getP);
}

std::unique_ptr<IActorState> BotTransformState::update() const {
    if (bot->getHp() == 0)
        return std::make_unique<BotDeadState>(bot);
    return nullptr;
}

void BotTransformState::exit() {}

} // namespace state
