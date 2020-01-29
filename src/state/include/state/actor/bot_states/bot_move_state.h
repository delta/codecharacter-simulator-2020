/**
 * @file bot_move_state.h
 * Declares the Bot Move State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

class STATE_EXPORT BotMoveState : public BotState {
  public:
    BotMoveState(Bot *bot);
    void enter() override;
    std::unique_ptr<IActorState> update() const override;
    void exit() override;
};

} // namespace state
