/**
 * @file shared_buffer.cpp
 * Definitions for buffer for shared memory
 */

#include "drivers/shared_memory_utils/shared_buffer.h"

#include <utility>

namespace drivers {

SharedBuffer::SharedBuffer(bool is_player_running,
                           int64_t turn_instruction_counter,
                           int64_t game_instruction_counter,
                           transfer_state::State transfer_state)
    : is_player_running(is_player_running),
      turn_instruction_counter(turn_instruction_counter),
      game_instruction_counter(game_instruction_counter),
      transfer_state(std::move(transfer_state)) {}
} // namespace drivers
