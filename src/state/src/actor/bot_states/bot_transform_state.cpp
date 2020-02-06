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
    // kill self and construct a tower
    bot->damage(bot->getHp());
    bot->constructTower(bot->getPlayerId(), bot->getPosition());
    bot->setTransforming(false);
}

std::unique_ptr<IActorState> BotTransformState::update() const {
    if (bot->getHp() == 0)
        return std::make_unique<BotDeadState>(bot);
    return nullptr;
}

void BotTransformState::exit() {}

} // namespace state
