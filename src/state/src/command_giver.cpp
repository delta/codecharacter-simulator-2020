/**
 * @file command_giver.cpp
 * Defines the class command_giver
 */

#include "state/command_giver.h"
#include "constants/actor.h"

namespace state {
CommandGiver::CommandGiver() = default;

CommandGiver::CommandGiver(ICommandTaker *state, logger::ILogger *logger)
    : state(state), logger(logger) {}

void CommandGiver::blastBot(ActorId actor_id, DoubleVec2D position) {
    state->blastBot(actor_id, position);
}

void CommandGiver::blastTower(ActorId actor_id) { state->blastTower(actor_id); }

void CommandGiver::transformBot(ActorId bot_id, DoubleVec2D position) {
    state->transformBot(bot_id, position);
}

void CommandGiver::moveBot(ActorId bot_id, DoubleVec2D position) {
    state->moveBot(bot_id, position);
}

DoubleVec2D CommandGiver::flipBotPosition(const Map &map,
                                          DoubleVec2D position) {
    size_t map_size = map.getSize();
    return {map_size - position.x, map_size - position.y};
}

DoubleVec2D CommandGiver::flipTowerPosition(const Map &map,
                                            DoubleVec2D position) {
    size_t map_size = map.getSize();
    return {(double) map_size - position.x, (double) map_size - position.y};
}

bool CommandGiver::isValidBotPosition(const Map &map, DoubleVec2D position) {
    double_t map_size = map.getSize();
    double_t x = position.x, y = position.y;
    bool within_map = (x >= 0 && x < map_size && y >= 0 && y < map_size);

    if (!within_map) {
        return false;
    }

    // If the bot can exist in any of the neighbouring blocks, it is a valid
    // position
    TerrainType type = map.getTerrainType(x, y);
    return (type == TerrainType::FLAG || type == TerrainType::LAND);
}

bool CommandGiver::isValidTowerPosition(const Map &map, DoubleVec2D position,
                                        PlayerId player_id) {
    auto tower_offset = getOffset(map, position, player_id);
    size_t map_size = map.getSize();
    return (tower_offset.x < map_size && tower_offset.x >= 0 &&
            tower_offset.y < map_size && tower_offset.y >= 0);
}

bool CommandGiver::hasBotStateChanged(
    BotStateName state_name, player_state::BotState player_state_name) {
    if (state_name == BotStateName::IDLE &&
        player_state_name != player_state::BotState::IDLE) {
        return true;
    } else if (state_name == BotStateName::TRANSFORM &&
               player_state_name != player_state::BotState::TRANSFORM) {
        return true;
    } else if (state_name == BotStateName::MOVE_TO_TRANSFORM &&
               player_state_name != player_state::BotState::TRANSFORM) {
        return true;
    } else if (state_name == BotStateName::BLAST &&
               player_state_name != player_state::BotState::BLAST) {
        return true;
    } else if (state_name == BotStateName::MOVE_TO_BLAST &&
               player_state_name != player_state::BotState::BLAST) {
        return true;
    } else if (state_name == BotStateName::MOVE &&
               player_state_name != player_state::BotState::MOVE) {
        return true;
    } else if (state_name == BotStateName::DEAD &&
               player_state_name != player_state::BotState::DEAD) {
        return true;
    }
    return false;
}

bool CommandGiver::hasTowerStateChanged(
    TowerStateName state_name, player_state::TowerState player_state_name) {
    if (state_name == TowerStateName::IDLE &&
        player_state_name != player_state::TowerState::IDLE) {
        return true;
    } else if (state_name == TowerStateName::BLAST &&
               player_state_name != player_state::TowerState::BLAST) {
        return true;
    } else if (state_name == TowerStateName::DEAD &&
               player_state_name != player_state::TowerState::DEAD) {
        return true;
    }
    return false;
}

bool CommandGiver::isSpawnOffset(const Map &map, DoubleVec2D position,
                                 PlayerId player_id) {
    Vec2D position_offset = getOffset(map, position, player_id);
    Vec2D player_1_base =
        getOffset(map, Constants::Map::PLAYER1_BASE_POSITION, player_id);
    Vec2D player_2_base =
        getOffset(map, Constants::Map::PLAYER2_BASE_POSITION, player_id);
    return (position_offset == player_1_base ||
            position_offset == player_2_base);
}

Vec2D CommandGiver::getOffset(const Map &map, DoubleVec2D position,
                              PlayerId player_id) {
    // FIXME : resolve type conflicts
    if (player_id == PlayerId::PLAYER1) {
        int64_t pos_x = std::floor(position.x), pos_y = std::floor(position.y);
        return {pos_x, pos_y};
    } else {
        int64_t pos_x = std::ceil(position.x) - 1,
                pos_y = std::ceil(position.y) - 1;
        return {pos_x, pos_y};
    }
}

DoubleVec2D CommandGiver::sanitize(DoubleVec2D position) {
    position.x = ceil(position.x * 1000000) / 1000000.0;
    position.y = ceil(position.y * 1000000) / 1000000.0;
    return position;
}

void CommandGiver::runCommands(
    std::array<player_state::State, 2> &player_states,
    std::array<bool, 2> skip_turn) {

    auto state_bots = state->getBots();
    auto state_towers = state->getTowers();
    auto map = state->getMap();

    // Validating and performing the tasks given by the player
    for (size_t id = 0; id < static_cast<size_t>(PlayerId::PLAYER_COUNT);
         ++id) {

        size_t enemy_id =
            (id + 1) % static_cast<int64_t>(PlayerId::PLAYER_COUNT);
        auto player_id = static_cast<PlayerId>(id);

        // If a player's turn should be skipped, don't process his moves
        if (skip_turn[id]) {
            logger->logError(player_id,
                             logger::ErrorType::EXCEED_TURN_INSTRUCTION_COUNT,
                             "Cannot exceed instruction count for each turn");
            continue;
        }

        // Checking player bots and state bots are of same length
        if ((state_bots[id].size() != player_states[id].bots.size()) ||
            (state_bots[enemy_id].size() !=
             player_states[id].enemy_bots.size())) {
            logger->logError(player_id,
                             logger::ErrorType::NUMBER_OF_BOTS_MISMATCH,
                             "Cannot add or delete bots in state");
            continue;
        }

        // Iterating through bots and assigning tasks to command taker
        for (size_t bot_index = 0; bot_index < state_bots[id].size();
             ++bot_index) {
            // Getting the player state bots and the state bots
            auto player_bot = player_states[id].bots[bot_index];
            auto state_bot = state_bots[id][bot_index];
            DoubleVec2D player_bot_position = player_bot.position;
            sanitize(player_bot_position);

            if (player_id == PlayerId::PLAYER2) {
                player_bot_position =
                    flipBotPosition(*map, player_bot_position);
            }

            // Finding which task the bot is trying to perform if any
            bool is_blasting = player_bot.blasting;
            bool is_transforming = player_bot.transforming;
            bool is_moving_to_blast = (bool) player_bot.final_destination;
            bool is_moving_to_transform =
                (bool) player_bot.transform_destination;
            bool is_moving = (bool) player_bot.destination;

            // Checking if the bot's properties have been changed
            if (player_bot.id != state_bot->getActorId()) {
                logger->logError(player_id,
                                 logger::ErrorType::NO_ALTER_BOT_PROPERTY,
                                 "Cannot alter bot's id");
                continue;
            } else if (player_bot.hp !=
                       static_cast<int64_t>(state_bot->getHp())) {
                logger->logError(player_id,
                                 logger::ErrorType::NO_ALTER_BOT_PROPERTY,
                                 "Cannot alter bot's hp");
                continue;
            } else if (player_bot_position != state_bot->getPosition()) {
                logger->logError(player_id,
                                 logger::ErrorType::NO_ALTER_BOT_PROPERTY,
                                 "Cannot alter bot's position");
                continue;
            }

            // Checking if the user modified the bot's state directly
            if (hasBotStateChanged(state_bot->getState(), player_bot.state)) {
                logger->logError(player_id,
                                 logger::ErrorType::NO_ALTER_BOT_PROPERTY,
                                 "Cannot alter bot's state");
                continue;
            }

            // Checking for multiple actions by a bot
            if (is_blasting + is_transforming + is_moving_to_blast +
                    is_moving_to_transform + is_moving >
                1) {
                logger->logError(
                    player_id, logger::ErrorType::NO_MULTIPLE_BOT_TASK,
                    "Cannot perform multiple bot tasks at the same time");
                continue;
            }

            if (is_blasting) {
                blastBot(player_bot.id, player_bot_position);
            } else if (is_transforming) {
                size_t num_towers = state_towers[id].size();
                if (num_towers >= Constants::Actor::MAX_NUM_TOWERS) {
                    logger->logError(player_id,
                                     logger::ErrorType::TOWER_LIMIT_REACHED,
                                     "Cannot build more towers than maximum "
                                     "number of towers");
                    continue;
                }
                if (isSpawnOffset(*map, player_bot_position, (PlayerId) id)) {
                    logger->logError(
                        player_id,
                        logger::ErrorType::INVALID_TRANSFORM_POSITION,
                        "Cannot transform in a spawn position");
                    continue;
                }
                transformBot(player_bot.id, player_bot_position);
            } else if (is_moving_to_blast) {
                // Validate the position where the player wants the bot to blast
                DoubleVec2D final_destination = player_bot.final_destination;
                sanitize(final_destination);
                if (player_id == PlayerId::PLAYER2) {
                    final_destination =
                        flipBotPosition(*map, final_destination);
                }
                if (!isValidBotPosition(*map, final_destination)) {
                    logger->logError(player_id,
                                     logger::ErrorType::INVALID_BLAST_POSITION,
                                     "Cannot blast bot in an invalid position");
                } else {
                    blastBot(player_bot.id, final_destination);
                }
            } else if (is_moving_to_transform) {
                // Validating the position where the player wants to transform
                // the bot
                DoubleVec2D transform_destination =
                    player_bot.transform_destination;
                sanitize(transform_destination);
                if (player_id == PlayerId::PLAYER2) {
                    transform_destination =
                        flipBotPosition(*map, transform_destination);
                }
                if (!isValidTowerPosition(*map, transform_destination,
                                          player_id)) {
                    logger->logError(
                        player_id,
                        logger::ErrorType::INVALID_TRANSFORM_POSITION,
                        "Cannot transform bot in invalid position");
                    continue;
                }
                size_t num_towers = state_towers[id].size();
                if (num_towers >= Constants::Actor::MAX_NUM_TOWERS) {
                    logger->logError(player_id,
                                     logger::ErrorType::TOWER_LIMIT_REACHED,
                                     "Cannot build more towers than maximum "
                                     "number of towers");
                    continue;
                }
                if (isSpawnOffset(*map, transform_destination, (PlayerId) id)) {
                    logger->logError(
                        player_id,
                        logger::ErrorType::INVALID_TRANSFORM_POSITION,
                        "Cannot transform in a spawn position");
                    continue;
                }
                transformBot(player_bot.id, transform_destination);
            } else if (is_moving) {
                // Validates the position that the player has requested to
                // move onto
                DoubleVec2D destination = player_bot.destination;
                sanitize(destination);
                if (player_id == PlayerId::PLAYER2) {
                    destination = flipBotPosition(*map, destination);
                }
                if (!isValidBotPosition(*map, destination)) {
                    logger->logError(player_id,
                                     logger::ErrorType::INVALID_MOVE_POSITION,
                                     "Cannot move to invalid position");
                    continue;
                } else {
                    moveBot(player_bot.id, destination);
                }
            }
        }

        // Checking if number of towers has been changed by towers
        if ((state_towers[id].size() != player_states[id].towers.size()) ||
            (state_towers[enemy_id].size() !=
             player_states[id].enemy_towers.size())) {
            logger->logError(player_id,
                             logger::ErrorType::NUMBER_OF_TOWERS_MISMATCH,
                             "Cannot add or erase towers in state");
            continue;
        }

        // Iterating through the towers and assigning tasks
        for (size_t tower_index = 0; tower_index < state_towers[id].size();
             ++tower_index) {
            // Getting the player state towers and the state towers
            auto player_tower = player_states[id].towers[tower_index];
            auto state_tower = state_towers[id][tower_index];
            DoubleVec2D player_tower_position = player_tower.position;
            sanitize(player_tower_position);
            if (player_id == PlayerId::PLAYER2) {
                player_tower_position =
                    flipTowerPosition(*map, player_tower_position);
            }

            auto player_id = static_cast<PlayerId>(id);

            // Finding which task the tower is trying to perform if any
            bool is_blasting = player_tower.blasting;

            // Checking if the player changed the tower directly
            if (player_tower.id != state_tower->getActorId()) {
                logger->logError(player_id,
                                 logger::ErrorType::NO_ALTER_TOWER_PROPERTY,
                                 "Cannot alter tower's id");
                continue;
            } else if (player_tower_position != state_tower->getPosition()) {
                logger->logError(player_id,
                                 logger::ErrorType::NO_ALTER_TOWER_PROPERTY,
                                 "Cannot alter the tower's position");
                continue;
            } else if (player_tower.hp !=
                       static_cast<int64_t>(state_tower->getHp())) {
                logger->logError(player_id,
                                 logger::ErrorType::NO_ALTER_TOWER_PROPERTY,
                                 "Cannot alter the tower's hp");
                continue;
            }

            // Checking if the user has changed the tower state directly
            if (hasTowerStateChanged(state_tower->getState(),
                                     player_tower.state)) {
                logger->logError(player_id,
                                 logger::ErrorType::NO_ALTER_TOWER_PROPERTY,
                                 "Cannot alter the tower's state");
                continue;
            }

            if (is_blasting) {
                uint64_t tower_age = state_tower->getAge();
                if (tower_age >= Constants::Actor::TOWER_MIN_BLAST_AGE) {
                    blastTower(player_tower.id);
                } else {
                    logger->logError(player_id,
                                     logger::ErrorType::NO_EARLY_BLAST_TOWER,
                                     "Cannot blast a tower before minimum "
                                     "blast age is reached");
                    continue;
                }
            }
        }
    }
}
} // namespace state
