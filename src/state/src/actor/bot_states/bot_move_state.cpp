/**
 * @file bot_move_state.cpp
 * Definition of bot move state
 */
#include "state/actor/bot_states/bot_move_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_blast_state.h"
#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot_states/bot_idle_state.h"
#include "state/actor/bot_states/bot_state.h"
#include "state/actor/bot_states/bot_transform_state.h"

namespace state {

BotMoveState::BotMoveState(Bot *bot) : BotState(BotStateName::MOVE, bot) {}

void BotMoveState::enter() {}

std::unique_ptr<IActorState> BotMoveState::update() const {

    if (bot->getHp() == 0) {
        return std::make_unique<BotDeadState>(bot);
    }

    if (bot->isDestinationSet() &&
        bot->getPosition() == bot->getDestination()) {
        return std::make_unique<BotIdleState>(bot);
    }

    if (bot->isFinalDestinationSet() &&
        bot->getPosition() == bot->getFinalDestination()) {
        return std::make_unique<BotBlastState>(bot);
    }

    if (bot->isTransformDestinationSet() &&
        bot->getPosition() == bot->getFinalDestination()) {
        return std::make_unique<BotTransformState>(bot);
    }

    // TODO: implement bot movement
    // bot is still moving

    // get pathplanner

    // get next position here

    // set next position here

    // no state change.
    return nullptr;
}

void BotMoveState::exit() {
    bot->clearDestination();
    bot->clearFinalDestination();
    bot->clearTransformDestination();
}

} // namespace state
