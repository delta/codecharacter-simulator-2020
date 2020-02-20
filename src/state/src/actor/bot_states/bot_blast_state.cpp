/**
 * @file bot_blast_state.cpp
 * Definition of bot blast state
 */

#include "state/actor/bot_states/bot_blast_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_dead_state.h"

namespace state {

BotBlastState::BotBlastState(Bot *bot) : BotState(BotStateName::BLAST, bot) {}

void BotBlastState::enter() {}

std::unique_ptr<IActorState> BotBlastState::update() const {
    if (bot->getHp() == 0) {
        return std::make_unique<BotDeadState>(bot);
    }
    return nullptr;
}

void BotBlastState::exit() {}

} // namespace state
