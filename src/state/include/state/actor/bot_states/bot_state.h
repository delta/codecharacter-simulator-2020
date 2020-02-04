/**
 * @file bot_state.h
 * @brief Declares interface for all Bot states
 */

#pragma once

#include "state/actor/bot.fwd.h"
#include "state/actor/interfaces/i_actor_state.h"

namespace state {

/**
 * @brief
 */
enum class BotStateName {
    /**
     * @brief Bot is currently moving
     */
    MOVE,
    /**
     * @brief Bot is currently transforming, transforming is set to true
     */
    TRANSFORM,
    /**
     * @brief Bot is blasting
     */
    BLAST,
    /**
     * @brief Bot is doing nothing, not moving, blasting or transforming
     */
    IDLE,
    /**
     * @brief Bot is dead.
     */
    DEAD
};

/**
 * @brief
 */
class STATE_EXPORT BotState : public IActorState {
  protected:
    /**
     * @brief Name of the state
     */
    BotStateName state_name;

    /**
     * @brief The bot object that this state is managing
     */
    Bot *bot;

  public:
    /**
     * @brief Construct a new BotState object
     *
     * @param state_name
     * @param state
     */
    BotState(BotStateName state_name, Bot *state);

    /**
     * @brief Getter function which returns the current state name of bot
     *
     * @return BotStateName
     */
    BotStateName getName() const;
};

} // namespace state
