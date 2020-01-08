/**
 * @file logger.h
 * Declaration for logger class
 */
#pragma once

// this is a generated file
#include "game.pb.h"

#include "logger/interfaces/i_logger.h"
#include "logger/logger_export.h"
#include "physics/vector.hpp"
#include "state/interfaces/i_command_taker.h"

#include <cstdint>
#include <ostream>

namespace logger {
class LOGGER_EXPORT Logger : public ILogger {
private:
  /**
   * State instance
   */
  state::ICommandTaker *state;

  /**
   * Number of turns since the start of the game
   */
  size_t turn_count;

  /**
   * Stores the instruction counts until they are written into the log along
   * with the remaininig state data, every turn
   */
  std::vector<size_t> instruction_counts;

  /**
   * Protobuf object holding complete game logs
   */
  std::unique_ptr<proto::Game> logs;

  /**
   *  Mapping from error messages to error codes
   */
  std::unordered_map<std::string, size_t> error_map;

  /**
   * Holds an incrementing value to assign each error a unique code
   */
  size_t current_error_code;

  /**
   * Holds the error codes that occured in a particular move for each player
   */
  std::array<std::vector<size_t>, 2> errors;

  /**
   * Number of instructions exceeding which the turn is forfeit
   */
  size_t player_instruction_limit_turn;

  /**
   * Number of instructions exceeding which the game is forfeit
   */
  size_t player_instruction_limit_game;

  /**
   * Max HP of bots
   */
  size_t bot_max_hp;

  /**
   * Max HP of tower
   */
  size_t tower_max_hp;

public:
  /**
   * Constructor for the Logger class
   */
  Logger(state::ICommandTaker *state, size_t player_instruction_limit_turn,
         size_t player_instruction_limit_game, size_t bot_max_hp,
         size_t tower_max_hp);

  /**
   * @see ILogger#LogState
   */
  void LogState() override;

  /**
   * @see ILogger#LogInstructionCount
   */
  void LogInstructionCount(state::PlayerId player_id, size_t count) override;

  /**
   * @see ILogger#LogError
   */
  void LogError(state::PlayerId player_id, ErrorType error_type,
                std::string message) override;

  /**
   * @see ILogger#LogFinalGameParams
   */
  void LogFinalGameParams(state::PlayerId player_id,
                          std::array<size_t, 2> final_scores) override;

  /**
   * @see ILogger#WriteGame
   * Defaults to std::cout when no stream passed
   */
  void WriteGame(std::ostream &write_stream = std::cout) override;
};
} // namespace logger
