/**
 * @file bot_dead_state.cpp
 * Definition of bot dead state
 */

#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

BotDeadState::BotDeadState(Bot *bot) : BotState(BotStateName::DEAD, bot) {}

void BotDeadState::enter() {}

std::unique_ptr<IActorState> BotDeadState::update() const { return nullptr; }

void BotDeadState::exit() {}

} // namespace state
