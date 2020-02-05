/**
 * @file bot_state.h
 *  Declares interface for all Bot states
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/interfaces/i_actor_state.h"

namespace state {

/**
 *
 */
enum class BotStateName {
    /**
     *  Bot is currently moving
     */
    MOVE,
    /**
     *  Bot is currently transforming, transforming is set to true
     */
    TRANSFORM,
    /**
     *  Bot is blasting
     */
    BLAST,
    /**
     *  Bot is doing nothing, not moving, blasting or transforming
     */
    IDLE,
    /**
     *  Bot is dead.
     */
    DEAD
};

/**
 *
 */
class STATE_EXPORT BotState : public IActorState {
  protected:
    /**
     *  Name of the state
     */
    BotStateName state_name;

    /**
     *  The bot object that this state is managing
     */
    Bot *bot;

  public:
    /**
     *  Construct a new BotState object
     *
     * @param state_name
     * @param state
     */
    BotState(BotStateName state_name, Bot *state);

    /**
     *  Getter function which returns the current state name of bot
     *
     * @return BotStateName
     */
    BotStateName getName() const;
};

} // namespace state
