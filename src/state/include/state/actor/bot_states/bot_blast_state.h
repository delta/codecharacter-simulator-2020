/**
 * @file bot_blash_state.h
 * @brief Declares the Bot Blast State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

class STATE_EXPORT BotBlastState : public BotState {
  public:
    BotBlastState(Bot *bot);
    void enter() override;
    std::unique_ptr<IActorState> update();
    void exit() override;
};

} // namespace state
