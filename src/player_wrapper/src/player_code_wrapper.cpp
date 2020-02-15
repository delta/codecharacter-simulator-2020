/**
 * @file player_code_wrapper.cpp
 * Contains definitions for the player code wrapper
 */

#include "player_wrapper/player_code_wrapper.h"
#include <sstream>

namespace player_wrapper {

PlayerCodeWrapper::PlayerCodeWrapper(std::unique_ptr<IPlayerCode> player_code)
    : player_code(std::move(player_code)) {}

std::string PlayerCodeWrapper::update(transfer_state::State &transfer_state) {
    using namespace transfer_state;

    auto player_state = ConvertToPlayerState(transfer_state);
    player_state = player_code->update(player_state);
    transfer_state = ConvertToTransferState(player_state);
    return player_code->getAndClearDebugLogs();
}
} // namespace player_wrapper
