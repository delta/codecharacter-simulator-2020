/**
 * @file state_syncer.cpp
 */

#include "state/state_syncer.h"

namespace state {
StateSyncer::StateSyncer() {}

StateSyncer::StateSyncer(std::unique_ptr<ICommandTaker> state,
                         std::unique_ptr<ICommandGiver> command_giver,
                         std::unique_ptr<logger::ILogger> logger)
    : state(std::move(state)), command_giver(std::move(command_giver)),
      logger(std::move(logger)) {}

void StateSyncer::updateMainState(
    std::array<player_state::State, 2> &player_states,
    std::array<bool, 2> skip_turns) {
    // Running the user's commands
    command_giver->runCommands(player_states, skip_turns);

    // Updating the main state
    state->update();

    // Updating the player states
    updatePlayerStates(player_states);
}

bool StateSyncer::isGameOver(PlayerId &winner) {
    return state->isGameOver(winner);
}

size_t StateSyncer::getPlayerId(size_t player_id, bool is_enemy) const {
    if (is_enemy) {
        return (player_id + 1) % (static_cast<size_t>(PlayerId::PLAYER_COUNT));
    }
    return static_cast<size_t>(player_id);
}

std::array<int64_t, 2> StateSyncer::getScores(bool game_over) {
    return state->getScores(game_over);
}

void StateSyncer::updatePlayerStates(
    std::array<player_state::State, 2> &player_states) {
    // Getting all the state information
    auto state_bots = state->getBots();
    auto state_towers = state->getTowers();
    auto map = state->getMap();
    std::array<std::array<player_state::MapElement, Constants::Map::MAP_SIZE>,
               Constants::Map::MAP_SIZE>
        player_map;

    // Creating a map of player_state map type
    for (size_t i = 0; i < map->getSize(); ++i) {
        for (size_t j = 0; j < map->getSize(); ++j) {
            auto &map_element = player_map[i][j];
            switch (map->getTerrainType(i, j)) {
            case TerrainType::LAND:
                map_element.type = player_state::TerrainType::LAND;
                break;
            case TerrainType::FLAG:
                map_element.type = player_state::TerrainType::FLAG;
                break;
            case TerrainType::WATER:
                map_element.type = player_state::TerrainType::WATER;
                break;
            case TerrainType::TOWER:
                map_element.type = player_state::TerrainType::TOWER;
                break;
            }
        }
    }

    // Iterating through players
    for (int64_t player_id = 0;
         player_id < static_cast<int>(PlayerId::PLAYER_COUNT); ++player_id) {

        // Getting the enemy id
        int64_t enemy_id = getPlayerId(player_id, true);

        // Assinging the attributes of the state bots and towers
        assignBots(player_id, player_states[player_id].bots, false);
        assignBots(enemy_id, player_states[player_id].enemy_bots, true);
        assignTowers(player_id, player_states[player_id].towers, false);
        assignTowers(enemy_id, player_states[player_id].enemy_towers, true);

        // Adding the player state map
        // For player1, positions need not be flipped
        if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
            for (int i = 0; i < map->getSize(); ++i) {
                std::copy(player_map[i].begin(), player_map[i].end(),
                          player_states[player_id].map[i].begin());
            }
        } else {
            size_t map_size = map->getSize();
            for (int i = 0; i < map_size; ++i) {
                for (int j = 0; j < map_size; ++j) {
                    player_states[player_id].map[i][j] =
                        player_map[map_size - 1 - i][map_size - 1 - j];
                }
            }
        }
    }

    // Logging the state
    logger->LogState();
}

// TODO : Change these functions to use the path planner to flip positions
DoubleVec2D StateSyncer::flipBotPosition(const Map *map, DoubleVec2D position) {
    size_t map_size = map->getSize();
    return DoubleVec2D(map_size - position.x, map_size - position.y);
}

Vec2D StateSyncer::flipTowerPosition(const Map *map, Vec2D position) {
    size_t map_size = map->getSize();
    return Vec2D(map_size - 1 - position.x, map_size - 1 - position.y);
}

void StateSyncer::assignBots(size_t id,
                             std::vector<player_state::Bot> player_bots,
                             bool is_enemy) {
    auto state_bots = state->getBots();
    auto map = state->getMap();
    size_t player_id = id;
    std::vector<player_state::Bot> new_bots;

    for (int bot_index = 0; bot_index < state_bots[player_id].size();
         ++bot_index) {
        // Creating a new bot with all attributes reset
        player_state::Bot new_bot;
        auto state_bot = state_bots[player_id][bot_index];

        new_bot.hp = state_bot->getHp();
        new_bot.position = DoubleVec2D::null;

        // Assinging the bot's state
        switch (state_bot->getState()) {
        case BotStateName::IDLE:
            new_bot.state = player_state::BotState::IDLE;
            break;
        case BotStateName::DEAD:
            new_bot.state = player_state::BotState::DEAD;
            break;
        case BotStateName::MOVE:
            new_bot.state = player_state::BotState::MOVE;
            break;
        case BotStateName::BLAST:
            new_bot.state = player_state::BotState::BLAST;
            break;
        case BotStateName::TRANSFORM:
            new_bot.state = player_state::BotState::TRANSFORM;
            break;
        case BotStateName::MOVE_TO_BLAST:
            new_bot.state = player_state::BotState::BLAST;
            break;
        case BotStateName::MOVE_TO_TRANSFORM:
            new_bot.state = player_state::BotState::TRANSFORM;
            break;
        }

        // Checking if it's player 2, then flipping the position
        player_id = getPlayerId(id, is_enemy);

        // For player one, the positions need not be inverted
        if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
            new_bot.position = state_bot->getPosition();
        } else {
            new_bot.position =
                flipBotPosition(state->getMap(), state_bot->getPosition());
        }

        // Adding the new bot to the list of new bots
        new_bots.push_back(new_bot);
    }

    // Moving the new player soldiers into their player state
    player_bots.clear();

    for (auto player_bot : new_bots) {
        player_bots.push_back(player_bot);
    }
}

DoubleVec2D StateSyncer::changeTowerToBotPosition(Vec2D position) {
    return DoubleVec2D(position.x, position.y);
}

Vec2D StateSyncer::changeBotToTowerPosition(DoubleVec2D position) {
    return Vec2D(std::floor(position.x), std::floor(position.y));
}

void StateSyncer::assignTowers(size_t id,
                               std::vector<player_state::Tower> player_towers,
                               bool is_enemy) {
    auto state_towers = state->getTowers();
    size_t player_id = id;
    std::vector<player_state::Tower> new_towers;

    for (int tower_index = 0; tower_index < state_towers[id].size();
         ++tower_index) {
        player_state::Tower new_tower;
        auto state_tower = state_towers[id][tower_index];
        new_tower.id = state_tower->getActorId();

        // Assigning the tower's state
        switch (state_tower->getState()) {
        case TowerStateName::IDLE:
            new_tower.state = player_state::TowerState::IDLE;
            break;
        case TowerStateName::DEAD:
            new_tower.state = player_state::TowerState::DEAD;
            break;
        case TowerStateName::BLAST:
            new_tower.state = player_state::TowerState::BLAST;
            break;
        }

        player_id = getPlayerId(id, is_enemy);

        if (static_cast<PlayerId>(player_id) == PlayerId::PLAYER1) {
            new_tower.position = state_tower->getPosition();
        } else {
            Vec2D tower_position =
                changeBotToTowerPosition(state_tower->getPosition());
            Vec2D flipped_tower_position =
                flipTowerPosition(state->getMap(), tower_position);
            new_tower.position =
                changeTowerToBotPosition(flipped_tower_position);
        }

        new_towers.push_back(new_tower);
    }

    player_towers.clear();

    for (auto player_tower : new_towers) {
        player_towers.push_back(player_tower);
    }
}

} // namespace state
