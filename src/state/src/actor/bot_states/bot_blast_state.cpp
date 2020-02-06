/**
 * @file bot_blast_state.cpp
 * Definition of bot blast state
 */

#include "state/actor/bot_states/bot_blast_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_dead_state.h"

namespace state {

BotBlastState::BotBlastState(Bot *bot) : BotState(BotStateName::BLAST, bot) {}

void BotBlastState::enter() {
    // kill self and inflict harm on others
    size_t bot_hp = bot->getHp();
    bot->damage(bot_hp);
    bot->damageEnemyActors(bot->getPlayerId(), bot->getActorId(),
                           bot->getPosition());
    bot->setBlasting(false);
}

std::unique_ptr<IActorState> BotBlastState::update() const {
    return std::make_unique<BotDeadState>(bot);
}

void BotBlastState::exit() {}

} // namespace state
