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
    NO_ALTER_ACTOR_ID,

    /**
     * Trying to blast an actor in an invalid position
     */
    INVALID_BLAST_POSITION,

    /**
     * Trying to transform a bot in an invalid position
     */
    INVALID_TRANSFORM_POSITION,

    /**
     * Cannot alter a tower's position
     */
    NO_ALTER_TOWER_POSITION
};

const std::vector<std::string> ErrorTypeName = {
    "INVALID_MOVE_POSITION",  "POSITION_OCCUPIED",
    "NO_MULTIPLE_BOT_TASK",   "NO_ALTER_ACTOR_ID",
    "INVALID_BLAST_POSITION", "INVALID_TRANSFORM_POSITION",
    "NO_ALTER_TOWER_POSITION"};

} // namespace logger
