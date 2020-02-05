/**
 * @file bot_state.cpp
 *  Defines the BotState class
 */

#include "state/actor/bot_states/bot_state.h"

namespace state {

BotState::BotState(BotStateName state_name, Bot *bot)
    : state_name(state_name), bot(bot) {}

BotStateName BotState::getName() const { return state_name; }

} // namespace state
