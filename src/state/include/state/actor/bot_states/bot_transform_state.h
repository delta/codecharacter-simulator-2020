/**
 * @file bot_transform_state.h
 * @brief Declares the Bot Transform State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

class STATE_EXPORT BotTransformState : public BotState {
  public:
    BotTransformState(Bot *bot);
    void enter() override;
    std::unique_ptr<IActorState> update() override;
    void exit() override;
};

} // namespace state
