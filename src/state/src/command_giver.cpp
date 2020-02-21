/**
 * @file command_giver.cpp
 * Defines the class command_giver
 */

#include "state/command_giver.h"

namespace state {
CommandGiver::CommandGiver() = default;

CommandGiver::CommandGiver(std::unique_ptr<ICommandTaker> state,
                           std::unique_ptr<logger::ILogger> logger)
    : state(std::move(state)), logger(std::move(logger)) {}

void CommandGiver::blastActor(PlayerId player_id, ActorId actor_id,
                              DoubleVec2D position) {
    state->blastActor(player_id, actor_id, position);
}

void CommandGiver::transformBot(PlayerId player_id, ActorId bot_id,
                                DoubleVec2D position) {
    state->transformBot(player_id, bot_id, position);
}

void CommandGiver::moveBot(PlayerId player_id, ActorId bot_id,
                           DoubleVec2D position) {
    state->moveBot(player_id, bot_id, position);
}

size_t CommandGiver::getMapSize() const {
    auto map = state->getMap();
    return map->getSize();
}

DoubleVec2D CommandGiver::flipBotPosition(DoubleVec2D position) const {
    size_t map_size = getMapSize();
    return {(long)map_size - position.x, (long)map_size - position.y};
}

Vec2D CommandGiver::flipTowerPosition(Vec2D position) const {
    size_t map_size = getMapSize();
    return {(long)map_size - 1 - position.x, (long)map_size - 1 - position.y};
}

bool CommandGiver::isValidBotPosition(DoubleVec2D position) const {
    size_t map_size = getMapSize();
    return (position.x >= 0 && position.x <= map_size && position.y >= 0 &&
            position.y <= map_size);
}

bool CommandGiver::isValidTowerPosition(Vec2D position) const {
    double_t map_size = getMapSize();
    return (position.x >= 0 && position.x < map_size && position.y >= 0 &&
            position.y < map_size);
}

bool CommandGiver::hasBotStateChanged(
    BotStateName state_name, player_state::BotState player_state_name) const {
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
    TowerStateName state_name,
    player_state::TowerState player_state_name) const {
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

void CommandGiver::runCommands(
    std::array<player_state::State, 2> &player_states,
    std::array<bool, 2> skip_turn) {

    auto state_bots = state->getBots();
    auto state_towers = state->getTowers();

    // Validating and performing the tasks given by the player
    for (size_t player_id = 0;
         player_id < static_cast<size_t>(PlayerId::PLAYER_COUNT); ++player_id) {
        // If a player's turn should be skipped, don't process his moves
        if (skip_turn[player_id]) {
            continue;
        }

        // Iterating through bots and assigning tasks to command taker
        for (size_t bot_index = 0;
             bot_index < player_states[player_id].bots.size(); ++bot_index) {
            // Getting the player state bots and the state bots
            auto player_bot = player_states[player_id].bots[bot_index];
            auto state_bot = state_bots[player_id][bot_index];

            auto Player_id = static_cast<PlayerId>(player_id);

            // Finding which task the bot is trying to perform if any
            bool is_blasting = player_bot.blasting;
            bool is_transforming = player_bot.transforming;
            bool is_moving_to_blast = (bool)player_bot.final_destination;
            bool is_moving_to_transform =
                (bool)player_bot.transform_destination;
            bool is_moving = (bool)player_bot.destination;

            // Checking if the bot's properties have been changed
            if (player_bot.id != state_bot->getActorId()) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_BOT_PROPERTY,
                                 "Cannot alter bot's id");
                continue;
            } else if (player_bot.hp != state_bot->getHp()) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_BOT_PROPERTY,
                                 "Cannot alter bot's hp");
                continue;
            } else if (player_bot.position != state_bot->getPosition()) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_BOT_PROPERTY,
                                 "Cannot alter bot's position");
                continue;
            }

            // Checking if the user modified the bot's state directly
            if (hasBotStateChanged(state_bot->getState(), player_bot.state)) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_BOT_PROPERTY,
                                 "Cannot alter bot's state");
                continue;
            }

            // Checking for multiple actions by a bot
            if (is_blasting + is_transforming + is_moving_to_blast +
                    is_moving_to_transform + is_moving >
                1) {
                logger->LogError(
                    Player_id, logger::ErrorType::NO_MULTIPLE_BOT_TASK,
                    "Cannot perform multiple bot tasks at the same time");
                continue;
            }

            if (is_blasting) {
                blastActor(Player_id, player_bot.id, player_bot.position);
            } else if (is_transforming) {
                transformBot(Player_id, player_bot.id, player_bot.position);
            } else if (is_moving_to_blast) {
                // Validate the position where the player wants the bot to blast
                DoubleVec2D target_position = player_bot.final_destination;
                if (!isValidBotPosition(target_position)) {
                    logger->LogError(Player_id,
                                     logger::ErrorType::INVALID_BLAST_POSITION,
                                     "Cannot blast bot in an invalid position");
                } else {
                    blastActor(Player_id, player_bot.id, target_position);
                }
            } else if (is_moving_to_transform) {
                // Validating the position where the player wants to transform
                // the bot
                DoubleVec2D target_position = player_bot.transform_destination;
                if (!isValidTowerPosition(target_position)) {
                    logger->LogError(
                        Player_id,
                        logger::ErrorType::INVALID_TRANSFORM_POSITION,
                        "Cannot transform bot in an invalid position");
                } else {
                    transformBot(Player_id, player_bot.id, target_position);
                }
            } else if (is_moving) {
                // Validates the position that the player has requested to move
                // onto
                DoubleVec2D target_position = player_bot.destination;
                if (!isValidBotPosition(target_position)) {
                    logger->LogError(Player_id,
                                     logger::ErrorType::INVALID_MOVE_POSITION,
                                     "Cannot move to invalid position");
                } else {
                    moveBot(Player_id, player_bot.id, target_position);
                }
            }
        }

        // Iterating through the towers and assigning tasks
        for (size_t tower_index = 0;
             tower_index < player_states[player_id].towers.size();
             ++tower_index) {
            // Getting the player state towers and the state towers
            auto player_tower = player_states[player_id].towers[tower_index];
            auto state_tower = state_towers[player_id][tower_index];

            auto Player_id = static_cast<PlayerId>(player_id);

            // Finding which task the tower is trying to perform if any
            bool is_blasting = player_tower.blasting;

            // Checking if the player changed the tower directly
            if (player_tower.id != state_tower->getActorId()) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_TOWER_PROPERTY,
                                 "Cannot alter tower's id");
                continue;
            } else if (player_tower.position != state_tower->getPosition()) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_TOWER_PROPERTY,
                                 "Cannot alter the tower's position");
                continue;
            } else if (player_tower.hp != state_tower->getHp()) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_TOWER_PROPERTY,
                                 "Cannot alter the tower's hp");
                continue;
            }

            // Checking if the user has changed the tower state directly
            if (hasTowerStateChanged(state_tower->getState(),
                                     player_tower.state)) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_TOWER_PROPERTY,
                                 "Cannot alter the tower's state");
                continue;
            }

            if (is_blasting) {
                blastActor(Player_id, player_tower.id, player_tower.position);
            }
        }
    }
}
} // namespace state
