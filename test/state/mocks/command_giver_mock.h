#pragma once

#include "state/interfaces/i_command_giver.h"
#include "state/player_state.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace std;
using namespace player_state;
using namespace state;

class CommandGiverMock : public ICommandGiver {
  public:
    MOCK_METHOD2(runCommands, void(array<player_state::State, 2> &player_states,
                                   array<bool, 2> skip_turn));
};
