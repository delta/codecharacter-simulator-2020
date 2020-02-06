/**
 * @file bot_transform_state.h
 * Declares the Bot Transform State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

class STATE_EXPORT BotTransformState : public BotState {
  public:
    /**
     * Construct a new Bot Transform State object
     *
     * @param bot
     */
    BotTransformState(Bot *bot);

    /**
     *
     */
    void enter() override;

    /**
     *
     * @return std::unique_ptr<IActorState>
     */
    std::unique_ptr<IActorState> update() const override;

    /**
     *
     */
    void exit() override;
};

} // namespace state
