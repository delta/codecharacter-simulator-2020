/**
 * file error_type.h
 * Defines a type of error to be logged
 */

#pragma once

#include <string>
#include <vector>

namespace logger {
/**
 * Enum for the different categories
 *
 * fill it as new categories are added
 */

enum class ErrorType {
  /**
   * TODO add error types here
   *
   */

  /**
   * Trying to move a soldier / villager to an invalid position
   */
  INVALID_MOVE_POSITION,
  /**
   * Trying to build a factory in a position that is already occupied
   */
  POSITION_OCCUPIED,
  /**
   * Attacking and building at the same time
   */
  NO_MULTIPLE_BOT_TASK,
  /**
   * Trying to change actor_id
   */
  NO_ALTER_ACTOR_ID
};

const std::vector<std::string> ErrorTypeName = {
    // TODO add the error names here in the same order
    "INVALID_MOVE_POSITION", "POSITION_OCCUPIED", "NO_MULTIPLE_BOT_TASK",
    "NO_ALTER_ACTOR_ID"};

} // namespace logger
