/**
 * @file state.h
 * Declarations for State which representing the internal state of the game
 */

#pragma once

#include "constants/constants.h"
#include "physics/vector.hpp"
#include "state/actor/bot.h"
#include "state/actor/tower.h"
#include "state/interfaces/i_command_taker.h"
#include "state/interfaces/i_updatable.h"
#include "state/map/map.h"
#include "state/path_planner/path_planner.h"
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
     * An instance of path planner
     */
    std::unique_ptr<PathPlanner> path_planner;

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
    void updateScores();

    /**
     * Returns the Actor By Id of the actor
     *
     * @param player_id
     * @param actor_id
     * @return Pointer to the actor
     * @return Nullptr
     */
    Actor *getActorById(PlayerId player_id, ActorId actor_id);

    /**
     * Returns the actor at given position if an actor exists at given point
     *
     * @param position
     * @return Actor* if actor exists in given position
     * @return Nullptr If no actor is at position
     */
    Actor *getActorByPosition(DoubleVec2D position);

    /**
     * Returns an offset From position object
     *
     * @param position Position for which offset is requested
     * @return Vec2D Offset
     */
    Vec2D getOffsetFromPosition(DoubleVec2D position);

    /**
     * Get the Blaster By Id
     *
     * @param id Actor id of the blaster
     * @return Blaster pointer
     */
    Blaster *getBlasterById(ActorId id);

    /**
     * Get the Tower By Id object
     *
     * @param player_id
     * @param actor_id
     * @return Tower*
     */
    Tower *getTowerById(PlayerId player_id, ActorId actor_id);

    /**
     * Get the Bot By Id object
     *
     * @param player_id
     * @param actor_id
     * @return Bot*
     */
    Bot *getBotById(PlayerId player_id, ActorId actor_id);

    /**
     * Get the Blaster by id
     *
     * @param player_id
     * @param actor_id
     * @return Blaster*
     */
    Blaster *getBlasterById(PlayerId player_id, ActorId actor_id);

    /**
     * Get the Impact Points when an actor blasts in a position
     *
     * @param position Position where blaster is blasting
     * @param impact_range Impact range where bot blast is affected
     * @return std::vector<Vec2D> Impact points
     */
    std::vector<Vec2D> getImpactPoints(DoubleVec2D position,
                                       int64_t impact_range);

    /**
     * Get the actors who get damage from position
     *
     * @param position
     * @param impact_range
     * @return std::vector<Actor *>
     */
    std::vector<Actor *> getAffectedActors(PlayerId player_id,
                                           DoubleVec2D position,
                                           int64_t impact_range);

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
     * Handles all transform requests and builds towers given situations
     */
    void handleTransformRequests();

    /**
     * Remove dead actors at the end of the turn
     */
    void removeDeadActors();

    /**
     * Spawn new bots for each player at the end of each turn
     */
    void spawnNewBots();

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
     * Callback passed to all blasters to damage neighbouring actors given a
     * position
     *
     * @param player_id Player id of actor calling callback
     * @param actor_id Actor id of actor calling callback
     * @param position Position where it wants to blast
     */
    void damageEnemyActors(PlayerId player_id, ActorId actor_id,
                           DoubleVec2D position);

    /**
     * Create a transform request for a given position
     *
     * @param bot Bot that is transforming into a tower
     */
    void createTower(Bot *bot);

    /**
     * @see ICommandTaker#getMap
     */
    const Map *getMap() const override;

    /**
     * @see ICommandTaker#getScores
     */
    const std::array<int64_t, 2> getScores(bool game_over) const override;

    /**
     * @see ICommandTaker#getTowers
     */
    const std::array<std::vector<Tower *>, 2> getTowers() override;

    /**
     * @see ICommandTaker#getBots
     */
    const std::array<std::vector<Bot *>, 2> getBots() override;

    void update() override;
};
} // namespace state
