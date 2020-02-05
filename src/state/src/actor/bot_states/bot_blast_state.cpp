/**
 * @file bot_blast_state.cpp
 * Definition of bot blast state
 */

#include "state/actor/bot_states/bot_blast_state.h"
#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot.h"

namespace state {

BotBlastState::BotBlastState(Bot *bot) : BotState(BotStateName::BLAST, bot) {}

void BotBlastState::enter() {
    // Inflict harm on onself
    bot->damage(bot->getHp());
}

std::unique_ptr<IActorState> BotBlastState::update() const {

    // TODO: inflict harm on bots/towers in range.

    return std::make_unique<BotDeadState>(bot);
}

void BotBlastState::exit() {}

} // namespace state
