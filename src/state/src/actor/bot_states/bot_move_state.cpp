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
#include "state/path_planner/path_planner.h"

namespace state {

BotMoveState::BotMoveState(Bot *bot) : BotState(BotStateName::MOVE, bot) {}

void BotMoveState::enter() {}

std::unique_ptr<IActorState> BotMoveState::update() const {
    using std::make_unique;

    // check if bot is dead
    if (bot->getHp() == 0) {
        return make_unique<BotDeadState>(bot);
    }

    if (!bot->isDestinationSet()) {
        // state transition based on characteristics
        if (bot->isFinalDestinationSet())
            return make_unique<BotMoveToBlastState>(bot);

        if (bot->isTransformDestinationSet())
            return make_unique<BotMoveToTransformState>(bot);

        if (bot->isBlasting())
            return make_unique<BotBlastState>(bot);

        if (bot->isTransforming())
            return make_unique<BotTransformState>(bot);

        // no other action done
        return make_unique<BotIdleState>(bot);
    }

    if (bot->getPosition() == bot->getDestination())
        return make_unique<BotIdleState>(bot);

    // bot is moving
    auto path_planner = bot->getPathPlanner();
    auto current_position = bot->getPosition();
    auto speed = bot->getSpeed();
    auto destination = bot->getDestination();
    auto next_position =
        path_planner->getNextPosition(current_position, destination, speed);

    if (next_position) {
        bot->setNewPosition(next_position);

        // transition to idle state, with new position already set
        if (next_position == destination) {
            return make_unique<BotIdleState>(bot);
        }
    }

    // no state change.
    return nullptr;
}

void BotMoveState::exit() { bot->clearDestination(); }

} // namespace state
