/**
 * @file bot_dead_state.cpp
 * Definition of bot dead state
 */

#include "state/actor/bot_states/bot_dead_state.h"
#include "state/actor/bot.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

BotDeadState::BotDeadState(Bot *bot) : BotState(BotStateName::DEAD, bot) {}

void BotDeadState::enter() {
    // If bot is in a flag area, updating score manager
    DoubleVec2D position = bot->getPosition();
    auto path_planner = bot->getPathPlanner();
    auto score_manager = bot->getScoreManager();

    if (path_planner->getTerrainType(position) == TerrainType::FLAG) {
        score_manager->actorExitedFlagArea(ActorType::BOT, bot->getPlayerId());
    }
}

std::unique_ptr<IActorState> BotDeadState::update() const { return nullptr; }

void BotDeadState::exit() {}

} // namespace state
