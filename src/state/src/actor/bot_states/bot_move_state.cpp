/**
 * @file bot_move_state.cpp
 * Definition of bot move state
 */
#include "state/actor/bot_states/bot_move_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_blast_state.h"
#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot_states/bot_idle_state.h"
#include "state/actor/bot_states/bot_move_to_blast_state.h"
#include "state/actor/bot_states/bot_move_to_transform_state.h"
#include "state/actor/bot_states/bot_state.h"
#include "state/actor/bot_states/bot_transform_state.h"

namespace state {

BotMoveState::BotMoveState(Bot *bot) : BotState(BotStateName::MOVE, bot) {}

void BotMoveState::enter() {}

std::unique_ptr<IActorState> BotMoveState::update() const {
    using std::make_unique;

    // check if bot is dead
    if (bot->getHp() == 0) {
        return make_unique<BotDeadState>(bot);
    }
    // when destination is not set
    if (!bot->isDestinationSet()) {
        // when final_destination is set
        if (bot->isFinalDestinationSet())
            return make_unique<BotMoveToBlastState>(bot);

        // when transform_destination is set
        if (bot->isTransformDestinationSet())
            return make_unique<BotMoveToTransformState>(bot);

        // when is_blasting is set to true
        if (bot->isBlasting())
            return make_unique<BotBlastState>(bot);

        // when is_transforming is set to true
        if (bot->isTransforming())
            return make_unique<BotTransformState>(bot);

        // no other action done
        return make_unique<BotIdleState>(bot);
        return nullptr;
    }

    if (bot->getPosition() == bot->getDestination())
        return make_unique<BotIdleState>(bot);

    // bot is still moving
    // TODO: implement bot movement

    // get pathplanner

    // get next position from path planner

    // set next position here, which updates bot position in lateUpdate

    // no state change.
    return nullptr;
}

void BotMoveState::exit() {
    bot->clearDestination();
    bot->clearFinalDestination();
    bot->clearTransformDestination();
}

} // namespace state
