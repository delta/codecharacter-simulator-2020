/**
 * @file bot_move_transform_state.h
 * Declares the Bot Move_Transform State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

class STATE_EXPORT BotMoveTransformState : public BotState {
  public:
    BotMoveTransformState(Bot *bot);
    void enter() override;
    std::unique_ptr<IActorState> update() const override;
    void exit() override;
};

} // namespace state
