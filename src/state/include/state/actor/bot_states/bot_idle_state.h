/**
 * @file bot_idle_state.h
 * @brief Declares the Bot Idle State class
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 * @brief
 */
class STATE_EXPORT BotIdleState : public BotState {
  public:
    /**
     * @brief Construct a new Bot Idle State object
     *
     * @param bot
     */
    BotIdleState(Bot *bot);

    /**
     * @brief
     */
    void enter() override;

    /**
     * @brief
     *
     * @return std::unique_ptr<IActorState>
     */
    std::unique_ptr<IActorState> update() const override;

    /**
     * @brief
     */
    void exit() override;
};

} // namespace state
