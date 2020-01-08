/**
 * @file logger.h
 * Declaration for logger class
 */
#pragma once

#include "logger/error_type.h"
#include "logger/interfaces/i_logger.h"
#include "logger/logger_export.h"
#include "physics/vector.hpp"
#include "state/interfaces/i_command_taker.h"

namespace logger {
class LOGGER_EXPORT Logger : public ILogger {

public:
  /**
   * Constructor for the Logger class
   */
  Logger();

  /**
   * @see ILogger#LogState
   */
  void LogState() override;

  /**
   * @see ILogger#LogInstructionCount
   */
  void LogInstructionCount(state::PlayerId player_id, int64_t count) override;

  /**
   * @see ILogger#LogError
   */
  void LogError(state::PlayerId player_id, ErrorType error_type,
                std::string message) override;

  /**
   * @see ILogger#LogFinalGameParams
   */
  void LogFinalGameParams(state::PlayerId player_id, bool was_deathmatch,
                          std::array<int64_t, 2> final_scores) override;

  /**
   * @see ILogger#WriteGame
   * Defaults to std::cout when no stream passed
   */
  void WriteGame(std::ostream &write_stream = std::cout) override;
};
} // namespace logger
