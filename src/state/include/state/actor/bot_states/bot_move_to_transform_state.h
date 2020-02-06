/**
 * @file bot_move_to_transform_state.h
 * Declares the Bot Move_Transform State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 *
 */
class STATE_EXPORT BotMoveToTransformState : public BotState {
  public:
    /**
     * Construct a new Bot Move To Transform State object
     *
     * @param bot instance
     */
    BotMoveToTransformState(Bot *bot);

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
