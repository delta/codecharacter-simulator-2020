/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#pragma once

#include "physics/vector.hpp"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/score_manager/score_manager.h"
#include "state/utilities.h"

namespace state {

class STATE_EXPORT State : public ICommandTaker {
  private:
    /**
     * Map instance that maintains game terrain
     */
    std::unique_ptr<Map> map;

    /**
     * Score Manager instance to maintain player score
     */
    std::unique_ptr<ScoreManager> score_manager;

    /**
     * Function to check whether the opponent is also requesting to change to
     * a tower at the same position
     *
     * @param[in] position		Position which is looked for in the
     * opponents build requests
     * @param[in] enemy_id		id of the enemy
     *
     */
    bool isPositionOccupied(Vec2D position, size_t enemy_id);

    /**
     * Compute scores for this turn, and record them
     */
    void UpdateScores();

  public:
    /**
     * Constructors
     */
    State();

    State(std::unique_ptr<Map> map,
          std::unique_ptr<ScoreManager> score_manager);

    /**
     * @see ICommandTaker#MoveBot
     */
    void moveBot(PlayerId player_id, ActorId actor_id,
                 DoubleVec2D position) override;

    /**
     * @see ICommandTaker#TransformBot
     */
    void transformBot(PlayerId player_id, ActorId bot_id) override;

    /**
     * @see ICommandTaker#BlastActor
     */
    void blastActor(PlayerId player_id, ActorId actor_id) override;

    /**
     * @see ICommandTaker#GetMap
     */
    const std::unique_ptr<Map> getMap() const override;

    /**
     * @see ICommandTaker#GetScores
     */
    const std::array<int64_t, 2> getScores(bool game_over) const override;

    /**
     * @see ICommandTaker#IsGameOver
     */
    bool isGameOver(PlayerId &winner) override;
};
} // namespace state
