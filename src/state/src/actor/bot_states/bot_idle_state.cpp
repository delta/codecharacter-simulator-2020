/**
 * @file bot_idle_state.cpp
 * @brief Definition of bot idle state
 */

#include "state/actor/bot_states/bot_idle_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

BotIdleState::BotIdleState(Bot *bot) : BotState(BotStateName::IDLE, bot) {}

void BotIdleState::enter() {}

std::unique_ptr<IActorState> BotIdleState::update() const {
    // TODO: finish implementation.
}

void BotIdleState::exit() {}

} // namespace state
