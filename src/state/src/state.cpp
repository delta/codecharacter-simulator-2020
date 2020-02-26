/**
 * @file state.cpp
 * Definitions for functions of the State class
 */

#include "state/state.h"

namespace state {
State::State() {}

State::State(std::unique_ptr<Map> map,
             std::unique_ptr<ScoreManager> score_manager,
             std::array<std::vector<std::unique_ptr<Bot>>, 2> bots,
             std::array<std::vector<std::unique_ptr<Tower>>, 2> towers)
    : map(std::move(map)), score_manager(std::move(score_manager)),
      bots(std::move(bots)), towers(std::move(towers)) {}

const Map *State::getMap() const { return map.get(); }

const std::array<uint64_t, 2> State::getScores() const {
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
const std::array<std::vector<T *>, 2> getRawPtrsFromUniquePtrs(
    std::array<std::vector<std::unique_ptr<T>>, 2> &actors) {
    std::array<std::vector<T *>, 2> ret_actors;

    for (int id = 0; id < static_cast<size_t>(PlayerId::PLAYER_COUNT); ++id) {
        std::vector<T *> actor_row;
        actor_row.resize(actors[id].size());

        for (size_t actor_index = 0; actor_index < actors[id].size();
             ++actor_index) {
            actor_row.push_back(actors[id][actor_index].get());
        }
        ret_actors[id] = actor_row;
    }
    return ret_actors;
}

Vec2D State::getOffsetFromPosition(DoubleVec2D position) {
    int64_t pos_x = std::floor(position.x), pos_y = std::floor(position.y);
    size_t map_size = map->getSize();

    if (pos_x == map_size) {
        --pos_x;
    }
    if (pos_y == map_size) {
        --pos_y;
    }

    return {pos_x, pos_y};
}

void State::handleTransformRequests() {
    // Creating a count array of actors in each position
    // Only if position counts of each postion is 1 for each build request, we
    // can approve the build request
    std::array<std::array<int64_t, Constants::Map::MAP_SIZE>,
               Constants::Map::MAP_SIZE>
        position_counts;

    for (int64_t id = 0; id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++id) {
        for (const auto &bot : bots[id]) {
            DoubleVec2D bot_position = bot->getPosition();
            Vec2D offset = getOffsetFromPosition(bot_position);
            position_counts[offset.y][offset.x]++;
        }

        for (const auto &tower : towers[id]) {
            DoubleVec2D tower_position = tower->getPosition();
            position_counts[tower_position.y][tower_position.x]++;
        }
    }

    // Iterating through transform request and checking which transform requests
    // to acknowledge and which transform requests to ignore
    for (int64_t id = 0; id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
         ++id) {
        auto player_id = static_cast<PlayerId>(id);
        for (const auto &request : transform_requests[id]) {
            int64_t bot_id = request->getActorId();
            auto bot = getBotById(player_id, bot_id);
            DoubleVec2D bot_position = bot->getPosition();
            Vec2D offset = getOffsetFromPosition(bot_position);

            // Checking if only one actor is in the offset position where
            // transforming is requested
            if (position_counts[offset.y][offset.x] == 1) {
                createTower(bot);
            }
        }
    }

    // Clearing the transform requests
    for (auto &requests : transform_requests) {
        requests.clear();
    }
}

void State::moveBot(PlayerId player_id, ActorId actor_id,
                    DoubleVec2D position) {
    // Setting the bot's destination
    auto bot = getActorById(player_id, actor_id);
    Unit *bot_as_unit = static_cast<Unit *>(bot);
    bot_as_unit->move(position);
}

void State::transformBot(PlayerId player_id, ActorId bot_id,
                         DoubleVec2D position) {
    // Adding a transform request to build a tower
    using namespace std::placeholders;
    auto get_actor_by_id = std::bind(&State::getActorById, this, _1, _2);
    int64_t id = static_cast<int64_t>(player_id);
    transform_requests[id].push_back(
        std::make_unique<TransformRequest>(player_id, bot_id, position));
}

std::vector<Actor *> State::getAffectedActors(PlayerId player_id,
                                              DoubleVec2D position,
                                              int64_t impact_range) {
    std::vector<Actor *> affected_actors;
    int64_t id = static_cast<int64_t>(player_id);
    int64_t enemy_id = (id + 1) % static_cast<int64_t>(PlayerId::PLAYER_COUNT);

    for (auto &bot : bots[enemy_id]) {
        DoubleVec2D bot_position = bot->getPosition();
        if (bot_position.distance(position) <= impact_range) {
            affected_actors.push_back(bot.get());
        }
    }

    for (auto &tower : towers[enemy_id]) {
        DoubleVec2D tower_position = tower->getPosition();
        if (tower_position.distance(position) <= impact_range) {
            affected_actors.push_back(tower.get());
        }
    }

    return affected_actors;
}

void State::damageEnemyActors(PlayerId player_id, ActorId actor_id,
                              DoubleVec2D position) {
    const Blaster *blaster = getBlasterById(player_id, actor_id);
    const Actor *actor = getActorById(player_id, actor_id);

    int64_t impact_radius = blaster->getBlastRange();
    int64_t damage_points = blaster->getBlastDamage();

    // Getting actors around position of size impact radius
    std::vector<Actor *> affected_actors =
        getAffectedActors(player_id, position, impact_radius);

    // Adding to the actor's damage incurred
    for (const auto &actor : affected_actors) {
        actor->damage(damage_points);
    }
}

const std::array<std::vector<Tower *>, 2> State::getTowers() {
    return getRawPtrsFromUniquePtrs(towers);
}

const std::array<std::vector<Bot *>, 2> State::getBots() {
    return getRawPtrsFromUniquePtrs(bots);
}

void State::spawnNewBots() {
    // Player 1 spawn
    using namespace std::placeholders;
    auto damage_enemy_actors =
        std::bind(&State::damageEnemyActors, this, _1, _2, _3);
    auto create_tower = std::bind(&State::createTower, this, _1);
    for (int64_t bot_index = 0;
         bot_index < Constants::Actor::BOT_SPAWN_FREQUENCY; ++bot_index) {
        bots[0].push_back(make_unique<Bot>(
            PlayerId::PLAYER1, Constants::Actor::BOT_MAX_HP,
            Constants::Actor::BOT_MAX_HP, Constants::Map::PLAYER1_BASE_POSITION,
            Constants::Actor::BOT_SPEED,
            Constants::Actor::BOT_BLAST_IMPACT_RADIUS,
            Constants::Actor::BOT_BLAST_DAMAGE_POINTS, score_manager.get(), path_planner.get(),
            damage_enemy_actors, create_tower));
    }

    // Player2 spawns
    for (int64_t bot_index = 0;
         bot_index < Constants::Actor::BOT_SPAWN_FREQUENCY; ++bot_index) {
        bots[1].push_back(make_unique<Bot>(
            PlayerId::PLAYER2, Constants::Actor::BOT_MAX_HP,
            Constants::Actor::BOT_MAX_HP, Constants::Map::PLAYER2_BASE_POSITION,
            Constants::Actor::BOT_SPEED,
            Constants::Actor::BOT_BLAST_IMPACT_RADIUS,
            Constants::Actor::BOT_BLAST_DAMAGE_POINTS, score_manager.get(), path_planner.get(),
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

    // Removing all the dead actors
    removeDeadActors();

    // Spawing bots at base positions
    spawnNewBots();

    // Updating the scores
    score_manager->updateScores();
}

void State::createTower(Bot *bot) {
    // Build a tower in given position with the same actor id as the bot and
    // transition the bot into dead state
    auto player_id = bot->getPlayerId();
    int64_t id = static_cast<int64_t>(player_id);
    auto bot_id = bot->getActorId();

    // TODO :
    // If the bot is in a flag area, then score_manager must be called and
    // actorExitedFlagArea

    // Finding ratio of hps of bot and tower to scale
    double scaling_ratio = (double) (Constants::Actor::BOT_MAX_HP) /
                           (double) (Constants::Actor::TOWER_MAX_HP);

    // Transitioning the bot into the dead state
    bot->setState(BotStateName::DEAD);

    // Creating a new tower for this player with the same actor id as the bot
    int64_t bot_hp = bot->getHp();
    int64_t scaled_hp = std::floor(scaling_ratio * bot_hp);
    using namespace std::placeholders;
    auto damage_enemy_actors =
        std::bind(&State::damageEnemyActors, this, _1, _2, _3);
    towers[id].push_back(
        make_unique<Tower>(player_id, scaled_hp, Constants::Actor::TOWER_MAX_HP,
                           bot->getPosition(), bot->getBlastDamage(),
                           bot->getBlastRange(), score_manager.get(), damage_enemy_actors));

    // TODO :
    // If tower is in flag area, then score_manager must be called and
    // actorEnteredFlagArea should be called
}

void State::blastActor(PlayerId player_id, ActorId actor_id,
                       DoubleVec2D position) {
    // Getting the actor and setting his blasting property as true
    Bot *bot = getBotById(player_id, actor_id);
    if (bot) {
        bot->setFinalDestination(position);
        return;
    }

    Tower *tower = getTowerById(player_id, actor_id);
    tower->setBlasting(true);
}

void State::removeDeadActors() {
    std::vector<std::unique_ptr<Actor>> actors_to_delete{};
    for (auto &state_bots : bots) {
        // Dividing the bots into dead and alive bots
        auto partition_point =
            std::stable_partition(state_bots.begin(), state_bots.end(),
                                  [](auto &s) { return s->getHp() != 0; });

        // Move all the dead soldiers into the dead actor buffer
        actors_to_delete.insert(actors_to_delete.end(),
                                std::make_move_iterator(partition_point),
                                std::make_move_iterator(state_bots.end()));

        // Delete the dead bots
        state_bots.erase(partition_point, state_bots.end());
    }

    for (auto &state_towers : towers) {
        // Dividing tower into dead and alive towers
        auto partition_point =
            std::stable_partition(state_towers.begin(), state_towers.end(),
                                  [](auto &s) { return s->getHp() != 0; });

        // Move all the dead soldiers into the dead actor buffer
        actors_to_delete.insert(actors_to_delete.end(),
                                std::make_move_iterator(partition_point),
                                std::make_move_iterator(state_towers.end()));

        // Delete the dead bots
        state_towers.erase(partition_point, state_towers.end());
    }
}

} // namespace state
