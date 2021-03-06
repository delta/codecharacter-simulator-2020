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
     * An instance of path planner
     */
    std::unique_ptr<PathPlanner> path_planner;

    Bot model_bot;

    Tower model_tower;

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
     * Returns the Actor By Id of the actor
     *
     * @param player_id
     * @param actor_id
     * @return Pointer to the actor
     * @return Nullptr
     */
    Actor *getActorById(ActorId actor_id);

    /**
     * Returns an offset from position
     *
     * @param position Position for which offset is requested
     * @return Vec2D Offset
     */
    Vec2D getOffsetFromPosition(DoubleVec2D position, PlayerId player_id);

    /**
     * Get the Blaster By Id
     *
     * @param id Actor id of the blaster
     * @return Blaster pointer
     */
    Blaster *getBlasterById(ActorId id);

    /**
     * Get the Tower By Id
     *
     * @param player_id
     * @param actor_id
     * @return Tower*
     */
    Tower *getTowerById(ActorId actor_id);

    /**
     * Get the Bot By Id
     *
     * @param player_id
     * @param actor_id
     * @return Bot*
     */
    Bot *getBotById(ActorId actor_id);

  public:
    /**
     * Constructors
     */
    State(std::unique_ptr<Map> map, std::unique_ptr<ScoreManager> score_manager,
          std::unique_ptr<PathPlanner> path_planner,
          std::array<std::vector<std::unique_ptr<Bot>>, 2> bots,
          std::array<std::vector<std::unique_ptr<Tower>>, 2> towers,
          Bot model_bot, Tower model_tower);

    /**
     * @see ICommandTaker#moveBot
     */
    void moveBot(ActorId actor_id, DoubleVec2D position) override;

    /**
     * Handles all transform requests and builds towers given situations
     */
    void handleTransformRequests();

    /**
     * Remove dead actors at the end of the turn
     */
    void removeDeadActors() override;

    /**
     * Spawn new bots for each player at the end of each turn
     */
    void spawnNewBots();

    /**
     * @see ICommandTaker#transformBot
     */
    void transformBot(ActorId bot_id, DoubleVec2D position) override;

    /**
     * @see ICommandTaker#blastBot
     */
    void blastBot(ActorId actor_id, DoubleVec2D position) override;

    /**
     * @see ICommandTaker#blastTower
     */
    void blastTower(ActorId actor_id) override;

    /**
     * Get the actors who get damage from position
     *
     * @param blast_position
     * @param impact_range
     * @return std::vector<Actor *>
     */
    std::vector<Actor *> getAffectedActors(PlayerId player_id,
                                           DoubleVec2D blast_position,
                                           size_t impact_range);

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
     * Taking model bot as reference, create new bot and add to list of player
     * bots
     * @param player_id Player to whom the bot belongs
     */
    void produceBot(PlayerId player_id);

    /**
     * Create a transform request for a given position
     *
     * @param bot Bot that is transforming into a tower
     */
    void constructTowerCallback(Bot *bot);

    /**
     * Convert a bot to tower, add to player tower list and kill bot
     *
     * @param bot Bot that is transforming into a tower
     */
    void produceTower(Bot *bot);

    /**
     * Overloaded function, which produces tower taking model_tower as reference
     * and adds it to player list
     * @param player_id Player to whom the tower belongs
     */
    void produceTower(PlayerId player_id);

    /**
     * @see ICommandTaker#getMap
     */
    Map *getMap() const override;

    /**
     * @see ICommandTaker#getScoreManager
     *
     * @return ScoreManager*
     */
    ScoreManager *getScoreManager() const override;

    /**
     * @see ICommandTaker#getPathPlanner
     *
     * @return PathPlanner*
     */
    PathPlanner *getPathPlanner() const override;

    /**
     * @see ICommandTaker#getScores
     */
    std::array<uint64_t, 2> getScores() const override;

    /**
     * @see ICommandTaker#getTowers
     */
    std::array<std::vector<Tower *>, 2> getTowers() override;

    /**
     * @see ICommandTaker#getBots
     */
    std::array<std::vector<Bot *>, 2> getBots() override;

    /**
     * Get the Transform Requests object
     *
     * @return std::array<std::vector<std::unique_ptr<TransformRequest>>, 2>
     */
    std::array<std::vector<TransformRequest *>, 2>
    getTransformRequests() override;

    /**
     * Updates the main state by calling update for each of the state actors
     * individually followed by updating scores and removing dead actors
     */
    void update() override;
};
} // namespace state
