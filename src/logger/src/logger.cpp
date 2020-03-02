/**
 * @file logger.cpp
 * Defines the logger class
 */

#include "logger/logger.h"
#include "constants/constants.h"
#include "state/interfaces/i_command_taker.h"

using namespace state;

#include <string>
#include <unordered_map>

using namespace state;

namespace logger {

Logger::Logger(ICommandTaker *state, size_t player_instruction_limit_turn,
               size_t player_instruction_limit_game, size_t bot_max_hp,
               size_t tower_max_hp)
    : state(state), turn_count(0), instruction_counts(std::vector<size_t>(
                                       (int) state::PlayerId::PLAYER_COUNT, 0)),
      logs(std::make_unique<proto::Game>()),
      error_map(std::unordered_map<std::string, size_t>()),
      current_error_code(0), errors(std::array<std::vector<size_t>, 2>()),
      player_instruction_limit_turn(player_instruction_limit_turn),
      player_instruction_limit_game(player_instruction_limit_game),
      bot_max_hp(bot_max_hp), tower_max_hp(tower_max_hp){};

proto::BotState GetProtoBotState(BotStateName bot_state) {
    proto::BotState curr_bot_state;

    switch (bot_state) {
    case BotStateName::MOVE:
        curr_bot_state = proto::BOT_MOVE;
        break;
    case BotStateName::MOVE_TO_TRANSFORM:
        curr_bot_state = proto::BOT_MOVE_TO_TRANSFORM;
        break;
    case BotStateName::TRANSFORM:
        curr_bot_state = proto::BOT_TRANSFORM;
        break;
    case BotStateName::BLAST:
        curr_bot_state = proto::BOT_BLAST;
        break;
    case BotStateName::MOVE_TO_BLAST:
        curr_bot_state = proto::BOT_MOVE_TO_BLAST;
        break;
    case BotStateName::IDLE:
        curr_bot_state = proto::BOT_IDLE;
        break;
    case BotStateName::DEAD:
        curr_bot_state = proto::BOT_DEAD;
        break;
    }

    return curr_bot_state;
}

proto::TowerState GetProtoTowerState(TowerStateName tower_state) {
    proto::TowerState curr_tower_state;

    switch (tower_state) {
    case TowerStateName::IDLE:
        curr_tower_state = proto::TOWER_IDLE;
        break;
    case TowerStateName::BLAST:
        curr_tower_state = proto::TOWER_BLAST;
        break;
    case TowerStateName::DEAD:
        curr_tower_state = proto::TOWER_DEAD;
        break;
    }

    return curr_tower_state;
}

proto::TerrainType GetProtoTerrainType(TerrainType terrain) {
    proto::TerrainType terrain_proto;

    switch (terrain) {
    case TerrainType::LAND:
        terrain_proto = proto::LAND;
        break;
    case TerrainType::WATER:
        terrain_proto = proto::WATER;
        break;
    case TerrainType::FLAG:
        terrain_proto = proto::FLAG;
        break;
    default:
        terrain_proto = proto::LAND;
    }

    return terrain_proto;
}

void Logger::logState() {
    turn_count++;
    auto *game_state = logs->add_states();

    auto bots = state->getBots();
    auto towers = state->getTowers();
    auto scores = state->getScores();
    auto score_manager = state->getScoreManager();

    // Things logged only in first turn
    if (turn_count == 1) {
        auto const *map = state->getMap();
        auto map_size = map->getSize();

        logs->set_map_size(map_size);
        std::vector<proto::TerrainType> map_terrain;
        for (size_t i = 0; i < map_size; i++) {
            for (size_t j = 0; j < map_size; j++) {
                logs->add_map_elements(
                    GetProtoTerrainType(map->getTerrainType(i, j)));
            }
        }

        logs->set_bot_max_hp(this->bot_max_hp);
        logs->set_tower_max_hp(this->tower_max_hp);
        logs->set_inst_limit_turn(this->player_instruction_limit_turn);
        logs->set_inst_limit_game(this->player_instruction_limit_game);
        logs->set_bot_speed(Constants::Actor::BOT_SPEED);
        logs->set_tower_blast_range(
            Constants::Actor::TOWER_BLAST_IMPACT_RADIUS);
        logs->set_bot_blast_range(Constants::Actor::BOT_BLAST_IMPACT_RADIUS);
    }

    // Log player bots
    for (auto const &player_bots : bots) {
        for (auto const &bot : player_bots) {
            proto::Bot *t_bot = game_state->add_bots();

            t_bot->set_id(bot->getActorId());
            t_bot->set_player_id((int) (bot->getPlayerId()));
            t_bot->set_hp(bot->getHp());
            t_bot->set_blast_range(bot->getBlastRange());
            t_bot->set_blast_bot(bot->isBlasting());
            t_bot->set_transform_bot(bot->isTransforming());

            auto bot_position = bot->getPosition();
            t_bot->set_x(bot_position.x);
            t_bot->set_y(bot_position.y);

            if (bot->isFinalDestinationSet()) {
                auto final_destination = bot->getFinalDestination();
                t_bot->set_target_x(final_destination.x);
                t_bot->set_target_y(final_destination.y);
            }

            if (bot->isTransformDestinationSet()) {
                auto transform_destination = bot->getTransformDestination();
                t_bot->set_target_x(transform_destination.x);
                t_bot->set_target_y(transform_destination.y);
            }

            t_bot->set_state(GetProtoBotState(bot->getState()));
        }
    }

    // Log player towers
    for (auto const &player_towers : towers) {
        for (auto const &tower : player_towers) {
            proto::Tower *t_tower = game_state->add_towers();

            t_tower->set_id(tower->getActorId());
            t_tower->set_player_id((int) (tower->getPlayerId()));
            t_tower->set_hp(tower->getHp());

            auto tower_position = tower->getPosition();
            t_tower->set_x(tower_position.x);
            t_tower->set_y(tower_position.y);

            t_tower->set_state(GetProtoTowerState(tower->getState()));
        }
    }

    // Log the player scores
    for (auto player_score : scores) {
        game_state->add_scores(player_score);
    }

    // Log instruction counts and reset to 0
    for (auto &inst_count : instruction_counts) {
        game_state->add_instruction_counts(inst_count);
        inst_count = 0;
    }

    // Log turn winners
    auto turn_winner = score_manager->getTurnWinner();
    switch (turn_winner) {
    case PlayerId::PLAYER1:
        game_state->set_turn_winner(proto::Winner::PLAYER1);
        break;
    case PlayerId::PLAYER2:
        game_state->set_turn_winner(proto::Winner::PLAYER2);
        break;
    case PlayerId::PLAYER_NULL:
        game_state->set_turn_winner(proto::Winner::TIE);
        break;
    }

    // Log the errors, clear the error vectors
    for (auto &player_errors : errors) {
        auto player_error_struct = game_state->add_player_errors();
        for (const auto &error_code : player_errors) {
            player_error_struct->add_errors(error_code);
        }
        player_errors.clear();
    }
};

void Logger::logInstructionCount(state::PlayerId player_id, size_t count) {
    this->instruction_counts[(int) player_id] = count;
};

void Logger::logError(state::PlayerId player_id, ErrorType error_type,
                      std::string message) {
    int64_t error_code;

    // Encode the message as "ERROR_TYPE: <message_string>"
    std::string full_error_message =
        ErrorTypeName[(int) error_type] + ": " + message;

    // If the message doesn't exist in the map, add an entry in the map and
    // increment the counter
    if (error_map.find(full_error_message) == error_map.end()) {
        error_code = current_error_code++;
        error_map[full_error_message] = error_code;
    } else {
        error_code = error_map[full_error_message];
    }

    errors[(int) player_id].push_back(error_code);
};

void Logger::logFinalGameParams(state::PlayerId player_id,
                                std::array<uint64_t, 2> final_scores) {
    // Write the error mapping to logs
    // Flip the mapping, int error_code -> string message
    for (const auto &element : error_map) {
        (*logs->mutable_error_map())[element.second] = element.first;
    }

    // Write the winner and game type
    switch (player_id) {
    case PlayerId::PLAYER1:
        logs->set_winner(proto::PLAYER1);
        break;
    case PlayerId::PLAYER2:
        logs->set_winner(proto::PLAYER2);
        break;
    default:
        logs->set_winner(proto::TIE);
        break;
    }

    // Set the final scores in the final game frame
    auto last_state = logs->mutable_states(logs->states_size() - 1);
    last_state->set_scores(0, final_scores[0]);
    last_state->set_scores(1, final_scores[1]);
};

void Logger::writeGame(std::ostream &write_stream) {
    logs->SerializeToOstream(&write_stream);
};

} // namespace logger
