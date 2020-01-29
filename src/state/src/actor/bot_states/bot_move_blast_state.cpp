/**
 * @file bot_move_blast_state.cpp
 * Defines the bot Move_Blast class
 */

#include "state/actor/bot_states/bot_move_blast_state.h"

namespace state {
BotMoveBlastState::BotMoveBlastState(Bot *bot)
    : BotState(BotStateName::MOVE_TO_BLAST, bot) {}
void BotMoveBlastState::enter() {}

std::unique_ptr<IActorState> BotMoveBlastState::update() const {}

void BotMoveBlastState::exit() {}
} // namespace state