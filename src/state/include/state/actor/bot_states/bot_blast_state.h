/**
 * @file bot_blast_state.h
 * Declares the Bot Blast State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 *  Declaration of Blast state class.
 */
class STATE_EXPORT BotBlastState : public BotState {
  public:
    /**
     * Construct a new Bot Blast State object
     *
     * @param bot instance
     */
    BotBlastState(Bot *bot);

    /**
     * @see IActorState#enter
     *
     * Damages enemy actors, sets its own hp to 0, resets isBlasting
     *
     */
    void enter() override;

    /**
     * Returns dead state
     *
     * @return std::unique_ptr<IActorState>
     */
    std::unique_ptr<IActorState> update() const override;

    /**
     * @see IActorState#exit
     */
    void exit() override;
};

} // namespace state
