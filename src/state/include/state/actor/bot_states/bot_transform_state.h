/**
 * @file bot_transform_state.h
 * Declares the Bot Transform State class
 */

#pragma once

#include "state/actor/bot_states/bot_state.h"
#include "state/actor/bot.fwd.h"

namespace state {

class STATE_EXPORT BotTransformState : public BotState {
  public:
    BotTransformState(Bot *bot);
    void enter() override;
    std::unique_ptr<IActorState> update() const override;
    void exit() override;
};

} // namespace state
