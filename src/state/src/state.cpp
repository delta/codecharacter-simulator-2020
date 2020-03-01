/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"
using namespace Constants::Actor;
using namespace Constants::Map;

namespace state {
State::State(std::unique_ptr<Map> map,
             std::unique_ptr<ScoreManager> score_manager,
             std::unique_ptr<PathPlanner> path_planner,
             std::array<std::vector<std::unique_ptr<Bot>>, 2> bots,
             std::array<std::vector<std::unique_ptr<Tower>>, 2> towers,
             Bot model_bot, Tower model_tower)
    : map(std::move(map)), score_manager(std::move(score_manager)),
      path_planner(std::move(path_planner)), bots(std::move(bots)),
      towers(std::move(towers)), model_bot(std::move(model_bot)),
      model_tower(std::move(model_tower)) {}

Map *State::getMap() const { return map.get(); }

std::array<uint64_t, 2> State::getScores() const {
    return score_manager->getScores();
}

ScoreManager *State::getScoreManager() const { return score_manager.get(); }

PathPlanner *State::getPathPlanner() const { return path_planner.get(); }

/**
 * Get the Raw Ptrs From Unique Ptrs object
 *
 * @tparam T Actor type that needs to be extracted
 * @param actors The actors' vectors
 * @return const std::array<std::vector<T *>, 2>
 */
template <typename T>
std::array<std::vector<T *>, 2> getRawPtrsFromUniquePtrs(
    std::array<std::vector<std::unique_ptr<T>>, 2> &actors) {
    auto ret_actors = std::array<std::vector<T *>, 2>{};

    for (size_t id = 0; id < static_cast<size_t>(PlayerId::PLAYER_COUNT);
         ++id) {
        std::vector<T *> actor_row;
        actor_row.resize(actors[id].size());

        for (size_t actor_index = 0; actor_index < actors[id].size();
             ++actor_index) {
            actor_row[actor_index] = actors[id][actor_index].get();
        }
        ret_actors[id] = actor_row;
    }
    return ret_actors;
}

Vec2D State::getOffsetFromPosition(DoubleVec2D position, PlayerId player_id) {
    if (player_id == PlayerId::PLAYER1) {
        int64_t pos_x = std::floor(position.x), pos_y = std::floor(position.y);
        return {pos_x, pos_y};
    } else {
        int64_t pos_x = std::ceil(position.x) - 1,
                pos_y = std::ceil(position.y) - 1;
        return {pos_x, pos_y};
    }
}

void State::handleTransformRequests() {

    // Creating a count array of bots in each position
    // Only if position counts of each position is 1 for each build request, we
    // can approve the build request
    // NOTE : We do not account for towers because bots cannot move to positions
    // with towers anyway

    std::array<std::array<int64_t, Constants::Map::MAP_SIZE>,
               Constants::Map::MAP_SIZE>
        position_counts{};

    for (int64_t id = 0; id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++id) {
        for (const auto &bot : bots[id]) {
            DoubleVec2D bot_position = bot->getPosition();
            Vec2D offset = getOffsetFromPosition(bot_position, (PlayerId) id);
            position_counts[offset.x][offset.y]++;
        }

        for (const auto &tower : towers[id]) {
            DoubleVec2D tower_position = tower->getPosition();
            Vec2D offset = getOffsetFromPosition(tower_position, (PlayerId) id);
            position_counts[offset.x][offset.y]++;
        }
    }

    // Iterating through transform request and checking which transform requests
    // to acknowledge and which transform requests to ignore
    for (int64_t id = 0; id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++id) {
        for (const auto &request : transform_requests[id]) {
            int64_t bot_id = request->getBotId();
            auto bot = getBotById(bot_id);
            DoubleVec2D bot_position = bot->getPosition();
            Vec2D offset = getOffsetFromPosition(bot_position, (PlayerId) id);
            // Checking if only one actor is in the offset position where
            // transforming is requested
            if (position_counts[offset.x][offset.y] == 1) {
                produceTower(bot);
            }
        }
    }

    // Clearing the transform requests
    for (auto &requests : transform_requests) {
        requests.clear();
    }
}

void State::moveBot(ActorId actor_id, DoubleVec2D position) {
    // Setting the bot's destination
    auto bot = getBotById(actor_id);
    bot->move(position);
}

void State::transformBot(ActorId bot_id, DoubleVec2D position) {
    // Adding a transform request to build a tower
    auto bot = getBotById(bot_id);
    bot->setTransformDestination(position);
}

std::vector<Actor *> State::getAffectedActors(PlayerId player_id,
                                              DoubleVec2D blast_position,
                                              size_t impact_range) {
    std::vector<Actor *> affected_actors;
    int64_t id = static_cast<int64_t>(player_id);
    int64_t enemy_id = (id + 1) % static_cast<int64_t>(PlayerId::PLAYER_COUNT);

    for (auto &bot : bots[enemy_id]) {
        DoubleVec2D bot_position = bot->getPosition();
        if (bot_position.distance(blast_position) <= impact_range) {
            affected_actors.push_back(bot.get());
        }
    }

    for (auto &tower : towers[enemy_id]) {
        DoubleVec2D tower_position = tower->getPosition();
        if (tower_position.distance(blast_position) <= impact_range) {
            affected_actors.push_back(tower.get());
        }
    }

    return affected_actors;
}

void State::damageEnemyActors(PlayerId player_id, ActorId actor_id,
                              DoubleVec2D position) {
    const Blaster *blaster = getBlasterById(actor_id);

    int64_t impact_radius = blaster->getBlastRange();
    int64_t damage_points = blaster->getBlastDamage();

    // Getting actors around position of size impact radius
    std::vector<Actor *> affected_actors =
        getAffectedActors(player_id, position, impact_radius);

    // Adding to the actor's damage incurred
    for (auto &affected_actor : affected_actors) {
        DoubleVec2D affected_actor_position = affected_actor->getPosition();
        double_t distance = position.distance(affected_actor_position);
        double_t remaining_distance = impact_radius - distance;
        double_t normalized_remaining_distance =
            remaining_distance / impact_radius;
        uint64_t inflicted_damage =
            damage_points * normalized_remaining_distance;
        affected_actor->damage(inflicted_damage);
    }
}

std::array<std::vector<Tower *>, 2> State::getTowers() {
    return getRawPtrsFromUniquePtrs(towers);
}

std::array<std::vector<Bot *>, 2> State::getBots() {
    return getRawPtrsFromUniquePtrs(bots);
}

std::array<std::vector<TransformRequest *>, 2> State::getTransformRequests() {
    return getRawPtrsFromUniquePtrs(transform_requests);
}

void State::spawnNewBots() {
    // Player 1 spawn
    using namespace std::placeholders;
    using namespace Constants::Actor;

    auto damage_enemy_actors =
        std::bind(&State::damageEnemyActors, this, _1, _2, _3);
    auto create_tower = std::bind(&State::constructTowerCallback, this, _1);

    // Number of bots to spawn for player 1, bots should be less than max num
    // bots
    auto num_spawn_bots_1 =
        std::min(BOT_SPAWN_FREQUENCY, MAX_NUM_BOTS - bots[0].size());
    // Number of bots to spawn for player 2
    auto num_spawn_bots_2 =
        std::min(BOT_SPAWN_FREQUENCY, MAX_NUM_BOTS - bots[1].size());

    for (size_t bot_index = 0; bot_index < num_spawn_bots_1; ++bot_index) {
        bots[0].push_back(
            make_unique<Bot>(PlayerId::PLAYER1, MAX_BOT_HP, MAX_BOT_HP,
                             Constants::Map::PLAYER1_BASE_POSITION, BOT_SPEED,
                             BOT_BLAST_IMPACT_RADIUS, BOT_BLAST_DAMAGE_POINTS,
                             score_manager.get(), path_planner.get(),
                             damage_enemy_actors, create_tower));
    }

    // Player2 spawns
    for (size_t bot_index = 0; bot_index < num_spawn_bots_2; ++bot_index) {
        bots[1].push_back(
            make_unique<Bot>(PlayerId::PLAYER2, MAX_BOT_HP, MAX_BOT_HP,
                             Constants::Map::PLAYER2_BASE_POSITION, BOT_SPEED,
                             BOT_BLAST_IMPACT_RADIUS, BOT_BLAST_DAMAGE_POINTS,
                             score_manager.get(), path_planner.get(),
                             damage_enemy_actors, create_tower));
    }
}

void State::update() {
    // Recalculate paths based on current obstacles
    path_planner->recomputePathGraph();

    // Update actors
    for (int64_t player_id = 0;
         player_id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++player_id) {
        for (const auto &bot : bots[player_id]) {
            bot->update();
        }
    }

    for (int64_t player_id = 0;
         player_id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++player_id) {
        for (const auto &tower : towers[player_id]) {
            tower->update();
        }
    }

    // Performing late updates for each actor
    for (int64_t player_id = 0;
         player_id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++player_id) {
        for (const auto &bot : bots[player_id]) {
            bot->lateUpdate();
        }
    }

    for (int64_t player_id = 0;
         player_id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++player_id) {
        for (const auto &tower : towers[player_id]) {
            tower->lateUpdate();
        }
    }

    // Resolving all calls to transform
    handleTransformRequests();

    // Updating the scores
    score_manager->updateScores();

    // Spawning bots at base positions
    spawnNewBots();
}

void State::constructTowerCallback(Bot *bot) {
    transform_requests[(int) bot->getPlayerId()].push_back(
        std::make_unique<TransformRequest>(
            bot->getPlayerId(), bot->getActorId(), bot->getPosition()));
}

void State::produceBot(PlayerId player_id) {
    using namespace std::placeholders;
    ConstructTowerCallback construct_tower_callback =
        std::bind(&State::constructTowerCallback, this, _1);
    BlastCallback blast_callback =
        std::bind(&State::damageEnemyActors, this, _1, _2, _3);

    auto bot = std::make_unique<Bot>(
        player_id, model_bot.getHp(), model_bot.getMaxHp(),
        PLAYER_BASE_POSITIONS[(int) player_id], model_bot.getSpeed(),
        model_bot.getBlastRange(), model_bot.getBlastDamage(),
        model_bot.getScoreManager(), model_bot.getPathPlanner(), blast_callback,
        construct_tower_callback);

    bots[(int) player_id].push_back(move(bot));
}

void State::produceTower(Bot *bot) {
    // Build a tower in given position with the same actor id as the bot and
    // transition the bot into dead state
    auto player_id = bot->getPlayerId();
    auto id = static_cast<int64_t>(player_id);
    auto bot_id = bot->getActorId();

    DoubleVec2D bot_offset =
        getOffsetFromPosition(bot->getPosition(), player_id);

    if (path_planner->getTerrainType(bot_offset, player_id) ==
        TerrainType::FLAG) {
        score_manager->actorExitedFlagArea(ActorType::BOT, player_id);
    }

    // Finding ratio of hps of tower and bot to scale
    double hp_ratio = (double) (Constants::Actor::MAX_TOWER_HP) /
                      (double) (Constants::Actor::MAX_BOT_HP);

    // Creating a new tower for this player with the same actor id as the bot
    int64_t bot_hp = bot->getHp();
    int64_t tower_hp = std::floor(hp_ratio * bot_hp);
    DoubleVec2D tower_position = bot_offset;

    // Add the tower obstacle in map
    path_planner->buildTower(bot_offset, PlayerId::PLAYER1);

    // Making the tower position as the center of the offset
    tower_position.x += 0.5;
    tower_position.y += 0.5;

    // Transitioning the bot into the dead state
    // Move the bot to the new tower's position
    bot->setState(BotStateName::DEAD);
    bot->setPosition(tower_position);

    using namespace std::placeholders;

    auto damage_enemy_actors =
        std::bind(&State::damageEnemyActors, this, _1, _2, _3);
    towers[id].push_back(make_unique<Tower>(
        bot_id, player_id, tower_hp, Constants::Actor::MAX_TOWER_HP,
        tower_position, Constants::Actor::TOWER_BLAST_DAMAGE_POINTS,
        Constants::Actor::TOWER_BLAST_IMPACT_RADIUS, score_manager.get(),
        damage_enemy_actors));

    if (path_planner->getTerrainType(tower_position, player_id) ==
        TerrainType::FLAG) {
        score_manager->actorEnteredFlagArea(ActorType::TOWER, player_id);
    }
}

void State::produceTower(PlayerId player_id) {
    using namespace std::placeholders;
    BlastCallback blast_callback =
        std::bind(&State::damageEnemyActors, this, _1, _2, _3);

    auto tower = std::make_unique<Tower>(
        player_id, model_tower.getHp(), model_tower.getMaxHp(),
        PLAYER_BASE_POSITIONS[(int) player_id], model_bot.getBlastDamage(),
        model_bot.getBlastRange(), model_bot.getScoreManager(), blast_callback);

    towers[(int) player_id].push_back(move(tower));
}

void State::blastBot(ActorId actor_id, DoubleVec2D position) {
    auto bot = getBotById(actor_id);
    bot->setFinalDestination(position);
}

void State::blastTower(ActorId actor_id) {
    auto tower = getTowerById(actor_id);
    tower->setBlasting(true);
}

void State::removeDeadActors() {
    for (auto &state_bots : bots) {
        // Dividing the bots into dead and alive bots
        auto partition_point = std::stable_partition(
            state_bots.begin(), state_bots.end(),
            [](auto &s) { return s->getState() != BotStateName::DEAD; });

        // Delete the dead bots
        state_bots.erase(partition_point, state_bots.end());
    }

    for (auto &state_towers : towers) {
        for (auto &tower : state_towers) {
            if (tower->getState() == TowerStateName::DEAD) {
                auto tower_position = tower->getPosition();
                tower_position.x -= 0.5;
                tower_position.y -= 0.5;
                path_planner->destroyTower(tower_position);
            }
        }
    }

    for (auto &state_towers : towers) {
        // Dividing tower into dead and alive towers
        auto partition_point = std::stable_partition(
            state_towers.begin(), state_towers.end(),
            [](auto &s) { return s->getState() != TowerStateName::DEAD; });

        // Delete the dead bots
        state_towers.erase(partition_point, state_towers.end());
    }
}

} // namespace state
