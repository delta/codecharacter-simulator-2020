/**
 * @file i_logger.h
 * Declaration for logger interface
 */

#pragma once

#include "logger/error_type.h"
#include "logger/logger_export.h"
#include "state/interfaces/i_command_taker.h"
#include <array>
#include <ostream>
#include <string>

namespace logger {
class LOGGER_EXPORT ILogger {
  public:
    virtual ~ILogger(){};
    /**
     * Logs all information from main state
     */
    virtual void logState() = 0;

    /**
     * Takes a player and corresponding instruction count, and logs it in the
     * current turn's game frame
     *
     * @param[in]   player_id   Player identifier
     * @param[in]   count       Instruction count
     */
    virtual void logInstructionCount(state::PlayerId player_id,
                                     size_t count) = 0;

    /**
     * Takes a player and the error, and logs it into the state. Every distinct
     * string is assigned an error code and stored in the error_map
     *
     * @param[in]   player_id    The player identifier
     * @param[in]   error_type   The error type
     * @param[in]   message      The error string
     */
    virtual void logError(state::PlayerId player_id, ErrorType error_type,
                          std::string message) = 0;

    /**
     * Logs final game parameters, should be called once, right before logging
     * state to stream (i.e before calling WriteGame)
     */
    virtual void logFinalGameParams(state::PlayerId player_id,
                                    std::array<uint64_t, 2> final_scores) = 0;

    /**
     * Writes the complete serialized logs to stream
     */
    virtual void writeGame(std::ostream &write_stream) = 0;
};
} // namespace logger
