/**
 * @file bot_idle_state.h
 *  Declares the Bot Idle State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 *  Declaration of Idle state of Bot
 */
class STATE_EXPORT BotIdleState : public BotState {
  public:
    /**
     *  Construct a new Bot Idle State object
     *
     * @param bot
     */
    BotIdleState(Bot *bot);

    /**
     * @see IActorState#enter
     */
    void enter() override;

    /**
     * Updates the bot state according to the characteristics
     *
     * @return std::unique_ptr<IActorState> a new state or nullptr
     */
    std::unique_ptr<IActorState> update() const override;

    /**
     * @see IActorState#exit
     */
    void exit() override;
};

} // namespace state
