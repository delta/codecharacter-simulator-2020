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
    size_t bot_hp = bot->getHp();
    bot->damage(bot_hp);
    bot->produceTower(bot->getPlayerId(), bot->getPosition());
}

std::unique_ptr<IActorState> BotTransformState::update() const {
    return std::make_unique<BotDeadState>(bot);
}

void BotTransformState::exit() {}

} // namespace state
