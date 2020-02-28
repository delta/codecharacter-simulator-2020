/**
 * @file player_result.h
 * Final result of a player
 */
#pragma once

#include "drivers/drivers_export.h"
#include "state/utilities.h"
#include <cstdint>
#include <iostream>
#include <vector>

namespace drivers {

/**
 * Final result of a player
 */
struct DRIVERS_EXPORT PlayerResult {
    /**
     * Status of the player
     */
    enum class Status {
        /**
         * Normal result
         */
        NORMAL,

        /**
         * A player took too long to finish their turn
         */
        TIMEOUT,

        /**
         * The player's code had a runtime error
         */
        RUNTIME_ERROR,

        /**
         * The player has exceeded the game instruction limit and thus
         * forfeited the game
         */
        EXCEEDED_INSTRUCTION_LIMIT,

        /**
         * The player's result is not valid (possible due to game timeout)
         */
        UNDEFINED
    };

    /**
     * The score of the player
     */
    uint64_t score;

    /**
     * The status of the player
     */
    Status status;
};

DRIVERS_EXPORT inline std::ostream &
operator<<(std::ostream &os, const PlayerResult::Status &status) {
    switch (status) {
    case PlayerResult::Status::NORMAL:
        os << "NORMAL";
        break;
    case PlayerResult::Status::TIMEOUT:
        os << "TIMEOUT";
        break;
    case PlayerResult::Status::RUNTIME_ERROR:
        os << "RUNTIME_ERROR";
        break;
    case PlayerResult::Status::EXCEEDED_INSTRUCTION_LIMIT:
        os << "EXCEEDED_INSTRUCTION_LIMIT";
        break;
    case PlayerResult::Status::UNDEFINED:
        os << "UNDEFINED";
        break;
    }

    return os;
}
} // namespace drivers
