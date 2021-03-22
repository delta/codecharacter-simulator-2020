#include "player_wrapper/transfer_state.h"
#include "state/player_state.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;
using namespace transfer_state;

class TransferStateTest : public Test {
  public:
    transfer_state::State ts;
    player_state::State ps;

    TransferStateTest() : ts(), ps() {
        ts.num_bots = 10;
        ts.num_enemy_bots = 20;
        ts.num_towers = 30;
        ps.num_bots = 50;
        ps.bots.assign(ps.num_bots, player_state::Bot());
        ps.num_enemy_bots = 30;
        ps.enemy_bots.assign(ps.num_enemy_bots, player_state::Bot());
    }
};

TEST_F(TransferStateTest, convertToPlayerStateTest) {
    auto new_ps = ConvertToPlayerState(ts);

    // non default values, changed in TransferStateTest constructor
    EXPECT_EQ(new_ps.num_bots, ts.num_bots);             // must equal 10
    EXPECT_EQ(new_ps.num_enemy_bots, ts.num_enemy_bots); // must equal 20
    EXPECT_EQ(new_ps.num_towers, ts.num_towers);         // must equal 30

    // default values
    EXPECT_EQ(new_ps.num_enemy_towers, ts.num_enemy_towers);
    EXPECT_EQ(new_ps.num_flags, ts.num_flags);
}

TEST_F(TransferStateTest, convertToTransferStateTest) {
    auto new_ts = ConvertToTransferState(ps);

    // non default values, changed in TransferStateTest constructor
    EXPECT_EQ(new_ts.num_bots, ps.num_bots);             // equals 50
    EXPECT_EQ(new_ts.num_enemy_bots, ps.num_enemy_bots); // equals 30

    // default values
    EXPECT_EQ(new_ts.num_towers, ps.num_towers);
    EXPECT_EQ(new_ts.num_enemy_towers, ps.num_enemy_towers);
    EXPECT_EQ(new_ts.num_flags, ps.num_flags);
}
