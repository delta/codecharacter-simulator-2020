/**
 * @file bot_move_blast_state.cpp
 * Defines the bot Move_Blast class
 */

#include "state/actor/bot_states/bot_move_to_blast_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_blast_state.h"
#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot_states/bot_idle_state.h"
#include "state/actor/bot_states/bot_move_state.h"
#include "state/actor/bot_states/bot_move_to_transform_state.h"
#include "state/actor/bot_states/bot_transform_state.h"

namespace state {
BotMoveToBlastState::BotMoveToBlastState(Bot *bot)
    : BotState(BotStateName::MOVE_TO_BLAST, bot) {}
void BotMoveToBlastState::enter() {}

std::unique_ptr<IActorState> BotMoveToBlastState::update() const {
    using std::make_unique;

    // check if bot is dead
    if (bot->getHp() == 0)
        return make_unique<BotDeadState>(bot);

    if (!bot->isFinalDestinationSet()) {
        // state transition based on characteristics
        if (bot->isDestinationSet())
            return make_unique<BotMoveState>(bot);

        if (bot->isTransformDestinationSet())
            return make_unique<BotMoveToTransformState>(bot);

        if (bot->isBlasting())
            return make_unique<BotBlastState>(bot);

        if (bot->isTransforming())
            return make_unique<BotTransformState>(bot);

        // no other action done
        return make_unique<BotIdleState>(bot);
    }

    // bot ready to blast
    if (bot->getPosition() == bot->getFinalDestination())
        return make_unique<BotBlastState>(bot);

    // bot is still moving
    auto path_planner = bot->getPathPlanner();
    auto current_position = bot->getPosition();
    auto speed = bot->getSpeed();
    auto final_destination = bot->getFinalDestination();
    auto next_position = path_planner->getNextPosition(
        current_position, final_destination, speed);

    if (next_position == current_position) {
        return make_unique<BotIdleState>(bot);
    }

    if (next_position) {
        bot->setNewPosition(next_position);

        // transition to blast state, with new position already set
        if (next_position == final_destination) {
            bot->clearFinalDestination();
            bot->setBlasting(true);
        }
    }
    // no state change
    return nullptr;
}

void BotMoveToBlastState::exit() { bot->clearFinalDestination(); }
} // namespace state
