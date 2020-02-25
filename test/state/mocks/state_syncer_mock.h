#include "state/interfaces/i_state_syncer.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace std;
using namespace state;

class StateSyncerMock : public IStateSyncer {
  public:
    MOCK_METHOD2(updateMainState,
                 void(std::array<player_state::State, 2> &player_states,
                      std::array<bool, 2> skip_turns));
    MOCK_METHOD1(updatePlayerStates,
                 void(std::array<player_state::State, 2> &player_states));
    MOCK_CONST_METHOD1(isGameOver, bool(PlayerId &winner));
    MOCK_CONST_METHOD1(getScores, array<int64_t, 2>(bool game_over));
};
