/**
 * @file bot_blash_state.h
 * Declares the Bot Blast State class
 */

#pragma once

#include "state/actor/bot_states/bot_state.h"

namespace state {

class STATE_EXPORT BotBlastState : public BotState {
  public:
    BotBlastState(Bot *bot);
    void enter() override;
    std::unique_ptr<IActorState> update() const override;
    void exit() override;
};

} // namespace state
