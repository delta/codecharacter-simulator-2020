/**
 * @file logger.cpp
 * Defines the logger class
 */

#include "logger/logger.h"
#include "state/interfaces/i_command_taker.h"

#include <string>

namespace logger {

Logger::Logger() { std::cout << "Logger Constructor called \n"; };

/**
 * @see ILogger#LogState
 */
void Logger::LogState() { std::cout << "LogState called \n"; };

/**
 * @see ILogger#LogInstructionCount
 */
void Logger::LogInstructionCount(state::PlayerId player_id, int64_t count) {
  std::cout << "LogInstructionCount called \n";
};

/**
 * @see ILogger#LogError
 */
void Logger::LogError(state::PlayerId player_id, ErrorType error_type,
                      std::string message) {
  std::cout << "LogError called \n";
};

/**
 * @see ILogger#LogFinalGameParams
 */
void Logger::LogFinalGameParams(state::PlayerId player_id) {
  std::cout << "LogFinalGameParams called \n";
};

/**
 * @see ILogger#WriteGame
 * Defaults to std::cout when no stream passed
 */
void Logger::WriteGame(std::ostream &write_stream) {
  std::cout << "Write Game called \n";
};

} // namespace logger
