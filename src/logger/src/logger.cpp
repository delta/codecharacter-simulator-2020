/**
 * @file logger.cpp
 * Defines the logger class
 */

#include "logger/logger.h"
#include "state/interfaces/i_command_taker.h"

using namespace state;

#include <string>

namespace logger {

Logger::Logger(ICommandTaker *state, size_t player_instruction_limit_turn,
               size_t player_instruction_limit_game, size_t bot_max_hp,
               size_t tower_max_hp)
    : state(state), turn_count(0), instruction_counts(std::vector<size_t>(
                                       (int)state::PlayerId::PLAYER_COUNT, 0)),
      logs(std::unique_ptr<proto::Game>()),
      error_map(std::unordered_map<std::string, size_t>()),
      current_error_code(0), errors(std::array<std::vector<size_t>, 2>()),
      player_instruction_limit_turn(player_instruction_limit_turn),
      player_instruction_limit_game(player_instruction_limit_game),
      bot_max_hp(bot_max_hp), tower_max_hp(tower_max_hp){};

void Logger::LogState() { std::cout << "LogState called \n"; };

void Logger::LogInstructionCount(state::PlayerId player_id, size_t count) {
    std::cout << "LogInstructionCount called \n";
};

void Logger::LogError(state::PlayerId player_id, ErrorType error_type,
                      std::string message) {
    std::cout << "LogError called \n";
};

void Logger::LogFinalGameParams(state::PlayerId player_id,
                                std::array<size_t, 2> final_scores) {
    std::cout << "LogFinalGameParams called \n";
};

void Logger::WriteGame(std::ostream &write_stream) {
    std::cout << "Write Game called \n";
};

} // namespace logger
