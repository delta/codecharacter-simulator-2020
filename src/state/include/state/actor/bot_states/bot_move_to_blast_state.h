/**
 * @file bot_move_to_blast_state.h
 * Declares the bot move_blast state
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {

/**
 *  Declaration of Move To Blast state of Bot
 */
class BotMoveToBlastState : public BotState {
  public:
    /**
     * Construct a new Bot Move To Blast State object
     *
     * @param bot instance
     */
    BotMoveToBlastState(Bot *bot);

    /**
     * @see IActorState#enter
     */
    void enter() override;

    /**
     * Moves bot towards final_destination and transitions to blast state
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
