/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#pragma once

#include "physics/vector.hpp"
#include "state/actor/bot.h"
#include "state/actor/tower.h"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/score_manager/score_manager.h"
#include "state/transform_request.h"
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
     * A list of bots indexed by player
     */
    std::array<std::vector<std::unique_ptr<Bot>>, 2> bots;

    /**
     * A list of towers indexed by player
     */
    std::array<std::vector<std::unique_ptr<Tower>>, 2> towers;

    /**
     * A list of build requests issued in each turn
     */
    std::array<std::vector<std::unique_ptr<TransformRequest>>, 2>
        transform_requests;

    /**
     * Function to check whether the opponent is also requesting to change to
     * a tower at the same position
     *
     * @param[in] position		Position which is looked for in the
     * opponents build requests
     * @param[in] enemy_id		id of the enemy
     *
     */
    bool isPositionOccupied(Vec2D position, uint64_t enemy_id);

    /**
     * Compute scores for this turn, and record them
     */
    void updateScores();

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
    State(std::unique_ptr<Map> map, std::unique_ptr<ScoreManager> score_manager,
          std::array<std::vector<std::unique_ptr<Bot>>, 2> bots,
          std::array<std::vector<std::unique_ptr<Tower>>, 2> towers);

    /**
     * @see ICommandTaker#moveBot
     */
    void moveBot(PlayerId player_id, ActorId actor_id,
                 DoubleVec2D position) override;

    /**
     * Handles all build requests and builds towers given situations
     */
    void handleBuildRequests();

    /**
     * @see ICommandTaker#transformBot
     */
    void transformBot(PlayerId player_id, ActorId bot_id,
                      DoubleVec2D position) override;

    /**
     * @see ICommandTaker#blastActor
     */
    void blastActor(PlayerId player_id, ActorId actor_id,
                    DoubleVec2D position) override;

    /**
     * @see ICommandTaker#getMap
     */
    const Map *getMap() const override;

    /**
     * @see ICommandTaker#getScores
     */
    const std::array<int64_t, 2> getScores(bool game_over) const override;

    /**
     * @see ICommandTaker#isGameOver
     */
    bool isGameOver(PlayerId &winner) override;

    /**
     * @see ICommandTaker#getTowers
     */
    const std::array<std::vector<Tower *>, 2> getTowers() override;

    /**
     * @see ICommandTaker#getBots
     */
    const std::array<std::vector<Bot *>, 2> getBots() override;
};
} // namespace state
