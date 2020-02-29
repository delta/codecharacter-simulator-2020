#pragma once

#include "constants/constants.h"
#include "player_code/player_code_export.h"
#include "player_wrapper/interfaces/i_player_code.h"

#include <iostream>
#include <vector>

namespace player_code {

/**
 * Class where player defines AI code
 */
class PLAYER_CODE_EXPORT PlayerCode1 : public player_wrapper::IPlayerCode {
    /**
     * Player AI update function (main logic of the AI)
     */
    player_state::State update(player_state::State state) override;
};
} // namespace player_code
