/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#pragma once

#include "state/state.h"

namespace state {
State::State() {}

State::State(std::unique_ptr<Map> map,
             std::unique_ptr<ScoreManager> score_manager)
    : map(std::move(map)), score_manager(std::move(score_manager)) {}

State::State(std::unique_ptr<Map> map,
             std::unique_ptr<ScoreManager> score_manager,
             std::array<std::vector<std::unique_ptr<Bot>>, 2> bots,
             std::array<std::vector<std::unique_ptr<Tower>>, 2> towers)
    : map(std::move(map)), score_manager(std::move(score_manager)),
      bots(std::move(bots)), towers(std::move(towers)) {}

Map *State::getMap() const { return map.get(); }

std::array<uint64_t, 2> State::getScores() const {
    return score_manager->getScores();
}

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
    std::array<std::vector<T *>, 2> ret_actors;

    for (size_t id = 0; id < static_cast<size_t>(PlayerId::PLAYER_COUNT);
         ++id) {
        std::vector<T *> actor_row;
        actor_row.resize(actors[id].size());

        for (size_t actor_index = 0; actor_index < actors[id].size();
             ++actor_index) {
            actor_row[id] = actors[id][actor_index].get();
        }
        ret_actors[id] = actor_row;
    }
    return ret_actors;
}

Vec2D State::getOffsetFromPosition(DoubleVec2D position) {
    int64_t pos_x = std::floor(position.x), pos_y = std::floor(position.y);

    return {pos_x, pos_y};
}

void State::handleTransformRequests() {
    // Creating a count array of bots in each position
    // Only if position counts of each position is 1 for each build request, we
    // can approve the build request
    // NOTE : We do not account for towers because bots cannot move to positions
    // with towers anyway

    std::array<std::array<int64_t, Constants::Map::MAP_SIZE>,
               Constants::Map::MAP_SIZE>
        position_counts;

    for (int64_t id = 0; id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++id) {
        for (const auto &bot : bots[id]) {
            DoubleVec2D bot_position = bot->getPosition();
            Vec2D offset = getOffsetFromPosition(bot_position);
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
            Vec2D offset = getOffsetFromPosition(bot_position);

            // Checking if only one actor is in the offset position where
            // transforming is requested
            if (position_counts[offset.x][offset.y] == 1) {
                createTower(bot);
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

void State::transformBot(PlayerId player_id, ActorId bot_id,
                         DoubleVec2D position) {
    // Adding a transform request to build a tower
    int64_t id = static_cast<int64_t>(player_id);
    transform_requests[id].push_back(
        std::make_unique<TransformRequest>(player_id, bot_id, position));
}

std::vector<Actor *> State::getAffectedActors(PlayerId player_id,
                                              DoubleVec2D blast_position,
                                              int64_t impact_range) {
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
        affected_actor->damage(damage_points);
    }
}

std::array<std::vector<Tower *>, 2> State::getTowers() {
    return getRawPtrsFromUniquePtrs(towers);
}

std::array<std::vector<Bot *>, 2> State::getBots() {
    return getRawPtrsFromUniquePtrs(bots);
}

void State::spawnNewBots() {
    // Player 1 spawn
    using namespace std::placeholders;
    using namespace Constants::Actor;

    auto damage_enemy_actors =
        std::bind(&State::damageEnemyActors, this, _1, _2, _3);
    auto create_tower = std::bind(&State::createTower, this, _1);
    for (size_t bot_index = 0; bot_index < BOT_SPAWN_FREQUENCY; ++bot_index) {
        bots[0].push_back(
            make_unique<Bot>(PlayerId::PLAYER1, BOT_MAX_HP, BOT_MAX_HP,
                             Constants::Map::PLAYER1_BASE_POSITION, BOT_SPEED,
                             BOT_BLAST_IMPACT_RADIUS, BOT_BLAST_DAMAGE_POINTS,
                             score_manager.get(), path_planner.get(),
                             damage_enemy_actors, create_tower));
    }

    // Player2 spawns
    for (size_t bot_index = 0; bot_index < BOT_SPAWN_FREQUENCY; ++bot_index) {
        bots[1].push_back(
            make_unique<Bot>(PlayerId::PLAYER2, BOT_MAX_HP, BOT_MAX_HP,
                             Constants::Map::PLAYER2_BASE_POSITION, BOT_SPEED,
                             BOT_BLAST_IMPACT_RADIUS, BOT_BLAST_DAMAGE_POINTS,
                             score_manager.get(), path_planner.get(),
                             damage_enemy_actors, create_tower));
    }
}

void State::update() {
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

void State::createTower(Bot *bot) {
    // Build a tower in given position with the same actor id as the bot and
    // transition the bot into dead state
    auto player_id = bot->getPlayerId();
    auto id = static_cast<int64_t>(player_id);
    auto bot_id = bot->getActorId();

    DoubleVec2D bot_position = bot->getPosition();
    if (path_planner->getTerrainType(bot_position) == TerrainType::FLAG) {
        score_manager->actorExitedFlagArea(ActorType::BOT, player_id);
    }

    // Finding ratio of hps of bot and tower to scale
    double hp_ratio = (double) (Constants::Actor::BOT_MAX_HP) /
                      (double) (Constants::Actor::TOWER_MAX_HP);

    // Transitioning the bot into the dead state
    bot->setState(BotStateName::DEAD);

    // Creating a new tower for this player with the same actor id as the bot
    int64_t bot_hp = bot->getHp();
    int64_t tower_hp = std::floor(hp_ratio * bot_hp);
    DoubleVec2D tower_position = getOffsetFromPosition(bot_position);

    // Making the tower position as the center of the offset
    tower_position.x += 0.5;
    tower_position.y += 0.5;

    using namespace std::placeholders;
    auto damage_enemy_actors =
        std::bind(&State::damageEnemyActors, this, _1, _2, _3);
    towers[id].push_back(make_unique<Tower>(
        bot_id, player_id, tower_hp, Constants::Actor::TOWER_MAX_HP,
        tower_position, Constants::Actor::TOWER_BLAST_DAMAGE_POINTS,
        Constants::Actor::TOWER_BLAST_IMPACT_RADIUS, score_manager.get(),
        damage_enemy_actors));

    if (path_planner->getTerrainType(tower_position) == TerrainType::FLAG) {
        score_manager->actorEnteredFlagArea(ActorType::TOWER, player_id);
    }
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
        auto partition_point =
            std::stable_partition(state_bots.begin(), state_bots.end(),
                                  [](auto &s) { return s->getHp() != 0; });

        // Delete the dead bots
        state_bots.erase(partition_point, state_bots.end());
    }

    for (auto &state_towers : towers) {
        // Dividing tower into dead and alive towers
        auto partition_point =
            std::stable_partition(state_towers.begin(), state_towers.end(),
                                  [](auto &s) { return s->getHp() != 0; });

        // Delete the dead bots
        state_towers.erase(partition_point, state_towers.end());
    }
}

} // namespace state
