/**
 * file error_type.h
 * Defines a type of error to be logged
 */

#pragma once

#include <string>
#include <vector>

namespace logger {

enum class ErrorType {
  /**
   * Trying to move a bot to an invalid position
   */
  INVALID_MOVE_POSITION,
  /**
   * Trying to build a tower in a position that is already occupied
   */
  POSITION_OCCUPIED,
  /**
   * Attacking and transforming at the same time
   */
  NO_MULTIPLE_BOT_TASK,
  /**
   * Trying to change actor_id
   */
  NO_ALTER_ACTOR_ID
};

const std::vector<std::string> ErrorTypeName = {
    "INVALID_MOVE_POSITION", "POSITION_OCCUPIED", "NO_MULTIPLE_BOT_TASK",
    "NO_ALTER_ACTOR_ID"};

} // namespace logger
