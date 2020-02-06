/**
 * @file bot_transform_state.h
 * Declares the Bot Transform State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 *  Declaration of Transform state of Bot
 */
class STATE_EXPORT BotTransformState : public BotState {
  public:
    /**
     * Construct a new Bot Transform State object
     *
     * @param bot
     */
    BotTransformState(Bot *bot);

    /**
     * Sets its hp to 0, constructs new tower at current position (implemented
     * through construct_tower callback)
     *
     * @see IActorState#enter
     */
    void enter() override;

    /**
     * Returns dead state bot
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
