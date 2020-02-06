/**
 * @file bot_dead_state.h
 * Declares the Bot Dead State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 *  Declaration of Dead State of Bot
 */
class STATE_EXPORT BotDeadState : public BotState {
  public:
    /**
     * Construct a new Bot Dead State object
     *
     * @param bot instance
     */
    BotDeadState(Bot *bot);

    /**
     * @see IActorState#enter
     */
    void enter() override;

    /**
     * Bot is dead. No state transition possible. Return a nullptr
     *
     * @return std::unique_ptr<IActorState> BotDeadState
     */
    std::unique_ptr<IActorState> update() const override;

    /**
     * @see IActorState#exit
     */
    void exit() override;
};

} // namespace state
