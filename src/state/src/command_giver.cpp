/**
 * @file command_giver.cpp
 * Defines the class command_giver
 */

#include "state/command_giver.h"

namespace state {
CommandGiver::CommandGiver() {}

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

DoubleVec2D CommandGiver::flipBotPosition(DoubleVec2D position) {
    size_t map_size = getMapSize();
    return DoubleVec2D(map_size - position.x, map_size - position.y);
}

Vec2D CommandGiver::flipTowerPosition(Vec2D position) const {
    size_t map_size = getMapSize();
    return Vec2D(map_size - 1 - position.x, map_size - 1 - position.y);
}

bool CommandGiver::isValidBotPosition(DoubleVec2D position) const {
    size_t map_size = getMapSize();
    return (position.x >= 0 && position.x <= map_size && position.y >= 0 &&
            position.y <= map_size);
}

bool CommandGiver::isValidTowerPosition(Vec2D position) const {
    size_t map_size = getMapSize();
    return (position.x >= 0 && position.x < map_size && position.y >= 0 &&
            position.y < map_size);
}

Vec2D CommandGiver::getTowerPositionFromBotPosition(
    DoubleVec2D position) const {
    size_t map_size = getMapSize();
    Vec2D grid_element(std::floor(position.x), std::floor(position.y));
    if (position.x == map_size) {
        grid_element.x = grid_element.x - 1;
    }
    if (position.y == map_size) {
        grid_element.y = grid_element.y - 1;
    }
    return grid_element;
}

void CommandGiver::runCommands(
    std::array<player_state::State, 2> &player_states,
    std::array<bool, 2> skip_turns) {

    // Getting all the state's actors and checking for valid actions
    auto state_bots = state->getBots();
    auto state_towers = state->getTowers();

    // Validating and performing the tasks given by the player
    for (size_t player_id = 0;
         player_id < static_cast<size_t>(PlayerId::PLAYER_COUNT); ++player_id) {
        // If a player's turn should be skipped, don't process his moves
        if (skip_turns[player_id]) {
            continue;
        }

        // Iterating through bots and assigning tasks to command taker
        for (size_t bot_index = 0;
             bot_index < player_states[player_id].bots.size(); ++bot_index) {
            // Getting the player state bots and the state bots
            auto player_bot = player_states[player_id].bots[bot_index];
            auto state_bot = state_bots[player_id][bot_index];

            PlayerId Player_id = static_cast<PlayerId>(player_id);

            // Finding which task the bot is trying to perform if any
            bool is_blasting = player_bot.blasting;
            bool is_transforming = player_bot.transforming;
            bool is_moving_to_blast =
                player_bot.final_destination != DoubleVec2D::null;
            bool is_moving_to_transform =
                player_bot.transform_destination != DoubleVec2D::null;
            bool is_moving = player_bot.destination != DoubleVec2D::null;

            // Validating the bot id
            if (player_bot.id != state_bot->getActorId()) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_ACTOR_ID,
                                 "Cannot alter bot's actor id");
                continue;
            }

            // Checking for multiple actions by a bot
            if (is_blasting + is_transforming + is_moving_to_blast +
                    is_moving_to_transform + is_moving >
                1) {
                logger->LogError(
                    Player_id, logger::ErrorType::NO_MULTIPLE_BOT_TASK,
                    "Cannot perform multiple bot tasks at the same time");
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
                                     "Cannot blast bot in invalid position");
                } else {
                    blastActor(Player_id, player_bot.id, target_position);
                }
            } else if (is_moving_to_transform) {
                // Validating the position where the player wants to transform
                // the bot
                DoubleVec2D target_position = player_bot.transform_destination;
                if (!isValidBotPosition(target_position)) {
                    logger->LogError(
                        Player_id, logger::ErrorType::INVALID_BLAST_POSITION,
                        "Cannot transform bot in invalid position");
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

        // Iterating through the towers and assinging tasks
        for (size_t tower_index = 0;
             tower_index < player_states[player_id].towers.size();
             ++tower_index) {
            // Getting the player state towers and the state towers
            auto player_tower = player_states[player_id].towers[tower_index];
            auto state_tower = state_towers[player_id][tower_index];

            PlayerId Player_id = static_cast<PlayerId>(player_id);

            // Finding which task the tower is trying to perform if any
            bool is_blasting = player_tower.blasting;

            // Checking for changing the actor id
            if (player_tower.id != state_tower->getActorId()) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_ACTOR_ID,
                                 "Cannot alter tower's actor id");
                continue;
            }

            // Checking if the player tries to alter the tower position
            if (player_tower.position != state_tower->getPosition()) {
                logger->LogError(Player_id,
                                 logger::ErrorType::NO_ALTER_TOWER_POSITION,
                                 "Cannot alter the tower's position");
                continue;
            }

            if (is_blasting) {
                blastActor(Player_id, player_tower.id, player_tower.position);
            }
        }
    }
}
} // namespace state
