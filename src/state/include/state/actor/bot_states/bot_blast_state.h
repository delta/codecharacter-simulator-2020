/**
 * @file bot_blash_state.h
 * Declares the Bot Blast State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 *
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
     */
    void enter() override;

    /**
     *
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
