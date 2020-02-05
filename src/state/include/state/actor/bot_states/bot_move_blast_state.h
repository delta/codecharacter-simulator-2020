/**
 * @file bot_move_blast_state.h
 * Declares the bot move_blast state
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/bot_states/bot_state.h"

namespace state {
class BotMoveBlastState : public BotState {
    BotMoveBlastState(Bot *bot);
    void enter() override;
    std::unique_ptr<IActorState> update() const override;
    void exit() override;
};
} // namespace state
