/**
 * @file bot_move_blast_state.cpp
 * Defines the bot Move_Transform class
 */

#include "state/actor/bot_states/bot_move_transform_state.h"

namespace state {
BotMoveTransformState::BotMoveTransformState(Bot *bot)
    : BotState(BotStateName::MOVE_TO_TRANSFORM, bot) {}
void BotMoveTransformState::enter() {}

std::unique_ptr<IActorState> BotMoveTransformState::update() const {}

void BotMoveTransformState::exit() {}
} // namespace state