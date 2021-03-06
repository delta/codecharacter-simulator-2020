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
     * Trying to blast an actor in an invalid position
     */
    INVALID_BLAST_POSITION,

    /**
     * Trying to transform a bot in an invalid position
     */
    INVALID_TRANSFORM_POSITION,

    /**
     * Cannot alter a player state tower's property directly
     */
    NO_ALTER_TOWER_PROPERTY,

    /**
     * Cannot alter a player state bot's property directly
     */
    NO_ALTER_BOT_PROPERTY,

    /**
     * Trying to build more towers than the maximum tower limit
     */
    TOWER_LIMIT_REACHED,

    /**
     * Mismatch in number of bots in state and player code
     */
    NUMBER_OF_BOTS_MISMATCH,

    /**
     * Mismatch in number of towers in state and player code
     */
    NUMBER_OF_TOWERS_MISMATCH,

    /**
     * Trying to blast a tower before minimum blast age
     */
    NO_EARLY_BLAST_TOWER,

    /**
     * Exceding instruction count per turn
     */
    EXCEED_TURN_INSTRUCTION_COUNT,
};

const std::vector<std::string> ErrorTypeName = {
    "INVALID_MOVE_POSITION",      "POSITION_OCCUPIED",
    "NO_MULTIPLE_BOT_TASK",       "INVALID_BLAST_POSITION",
    "INVALID_TRANSFORM_POSITION", "NO_ALTER_TOWER_PROPERTY",
    "NO_ALTER_BOT_PROPERTY",      "TOWER_LIMIT_REACHED",
    "NUMBER_OF_BOTS_MISMATCH",    "NUMBER_OF_TOWERS_MISMATCH",
    "NO_EARLY_BLAST_TOWER",       "EXCEED_TURN_INSTRUCTION_COUNT"};
} // namespace logger
