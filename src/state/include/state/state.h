/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#pragma once

#include "physics/vector.hpp"
#include "state/command_giver.h"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_updatable.h"
#include "state/state_syncer.h"
#include "state/utilities.h"

namespace state {

class STATE_EXPORT State : public ICommandTaker {
  private:
    /**
     * Map instance that maintains game terrain
     */
    std::unique_ptr<Map> map;

    /**
     * Function to check whether the opponent is also requesting to change to
     * a tower at the same posiion
     *
     * @param[in] position		Position which is looked for in the
     * oppenents
     * build requests
     * @param[in] enemy_id		id of the enemy
     *
     */
    bool IsPositionTaken(Vec2D position, ActorId enemy_id);

    /**
     * The current game scores
     */
    std::array<size_t, 2> scores;

    /**
     * Compute scores for this turn, and record them
     */
    void UpdateScores();

  public:
    /**
     * Constructors
     */
    State();

    State(std::unique_ptr<Map> map, std::array<size_t, 2> scores);

    /**
     * @see ICommandTaker#MoveBot
     */
    void MoveBot(PlayerId player_id, ActorId actor_id, Vec2D position) override;

    /**
     * @see ICommandTaker#TransformBot
     */
    void TransformBot(PlayerId player_id, ActorId bot_id) override;

    /**
     * @see ICommandTaker#BlastActor
     */
    void BlastActor(PlayerId player_id, ActorId actor_id) override;

    /**
     * @see ICommandTaker#GetMap
     */
    const std::unique_ptr<Map> GetMap() const override;

    /**
     * @see ICommandTaker#GetScores
     */
    const std::array<int64_t, 2> GetScores(bool game_over) const override;

    /**
     * @see ICommandTaker#IsGameOver
     */
    bool IsGameOver(PlayerId &winner) override;
};
} // namespace state
