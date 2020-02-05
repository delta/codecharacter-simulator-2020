/**
 * @file bot_idle_state.h
 *  Declares the Bot Idle State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 *  The Idle state class of bot
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
     *  Called right after the bot switches to this state
     */
    void enter() override;

    /**
     *  Performs state transitions
     *        If the bot blasts, then the state will transition to blast state
     * and dead state from blast state Else, remain in idle state if the bot
     * transforms, the state will transitions to transform state and goes to
     * idle state
     *
     * @return std::unique_ptr<IActorState> a new state
     */
    std::unique_ptr<IActorState> update() const override;

    /**
     *  Called right before the bot switches from this state to another
     * state
     */
    void exit() override;
};

} // namespace state
