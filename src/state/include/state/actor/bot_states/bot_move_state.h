/**
 * @file bot_move_state.h
 * Declares the Bot Move State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 *  Declaration of Move state of Bot
 */
class STATE_EXPORT BotMoveState : public BotState {
  public:
    /**
     * Construct a new Bot Move State object
     *
     * @param bot instance
     */
    BotMoveState(Bot *bot);

    /**
     * @see IActorState#enter
     */
    void enter() override;

    /**
     * Updates the bot position if destination is set
     * else returns a new state based on state characteristics
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
