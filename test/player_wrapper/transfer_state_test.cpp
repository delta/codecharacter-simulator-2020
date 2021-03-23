#include "player_wrapper/transfer_state.h"
#include "state/player_state.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;
using namespace transfer_state;

const auto L = player_state::TerrainType::LAND;
const auto W = player_state::TerrainType::WATER;
const auto F = player_state::TerrainType::FLAG;

class TransferStateTest : public Test {
  public:
    transfer_state::State ts;
    player_state::State ps;

    TransferStateTest() : ts(), ps() {
        ts.num_bots = 10;
        ts.num_enemy_bots = 20;
        ts.num_towers = 30;
        ts.scores = {1000, 2000};

        ps.num_bots = 50;
        ps.bots.assign(ps.num_bots, player_state::Bot());
        ps.num_enemy_bots = 30;
        ps.enemy_bots.assign(ps.num_enemy_bots, player_state::Bot());
        ps.scores = {100, 200};

        // Creating a basic map and player state
        vector<vector<player_state::TerrainType>> map = {{L, L, L, L, L},
                                                         {L, W, F, W, L},
                                                         {L, F, F, F, L},
                                                         {L, W, F, W, L},
                                                         {L, L, L, L, L}};

        // Creating a map of type MapElement from terrain
        for (size_t x = 0; x < MAP_SIZE; ++x) {
            for (size_t y = 0; y < MAP_SIZE; ++y) {
                ts.map[x][y].setTerrain(W);
                ps.map[x][y].setTerrain(W);
            }
        }

        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                ts.map[x][y].setTerrain(map[x][y]);
                ps.map[x][y].setTerrain(map[x][y]);
                if (map[x][y] == F) {
                    // updating flag offsets
                    ps.flag_offsets.push_back(DoubleVec2D(x, y));
                    ts.flag_offsets[ts.num_flags] = DoubleVec2D(x, y);
                    ts.num_flags++;
                    ps.num_flags++;
                }
            }
        }
    }

    // had to templatize this because ps.scores is array<int64_t,2> while
    // ts.scores is array<uint64_t,2>
    template <typename Integer>
    pair<int64_t, int64_t> pairFromScores(const array<Integer, 2> &scores) {
        return make_pair(scores[0], scores[1]);
    }

    bool mapEquality(
        const array<array<MapElement, Map::MAP_SIZE>, Map::MAP_SIZE> &map1,
        const array<array<MapElement, Map::MAP_SIZE>, Map::MAP_SIZE> &map2) {
        for (auto i = 0u; i < Map::MAP_SIZE; i++) {
            for (auto j = 0u; j < Map::MAP_SIZE; j++) {
                if (map1[i][j].type != map2[i][j].type) {
                    return false;
                }
            }
        }
        return true;
    }

    bool flagOffsetsEquality(
        const vector<DoubleVec2D> &ps_flags,
        const array<DoubleVec2D, Map::MAP_SIZE * Map::MAP_SIZE> &ts_flags) {
        for (auto i = 0u; i < ps_flags.size(); i++) {
            if (ps_flags[i] != ts_flags[i])
                return false;
        }
        return true;
    }
};

TEST_F(TransferStateTest, ConvertToPlayerStateTest) {
    auto new_ps = ConvertToPlayerState(ts);

    // non default values, changed in TransferStateTest constructor
    EXPECT_EQ(new_ps.num_bots, ts.num_bots);             // must equal 10
    EXPECT_EQ(new_ps.num_enemy_bots, ts.num_enemy_bots); // must equal 20
    EXPECT_EQ(new_ps.num_towers, ts.num_towers);         // must equal 30
    EXPECT_EQ(pairFromScores(new_ps.scores), pairFromScores(ts.scores));
    EXPECT_EQ(mapEquality(new_ps.map, ts.map), true);
    EXPECT_EQ(new_ps.num_flags, ts.num_flags);
    EXPECT_EQ(flagOffsetsEquality(new_ps.flag_offsets, ts.flag_offsets), true);

    // default values
    EXPECT_EQ(new_ps.num_enemy_towers, ts.num_enemy_towers);
}

TEST_F(TransferStateTest, ConvertToTransferStateTest) {
    auto new_ts = ConvertToTransferState(ps);

    // non default values, changed in TransferStateTest constructor
    EXPECT_EQ(new_ts.num_bots, ps.num_bots);             // equals 50
    EXPECT_EQ(new_ts.num_enemy_bots, ps.num_enemy_bots); // equals 30
    EXPECT_EQ(pairFromScores(new_ts.scores), pairFromScores(ps.scores));
    EXPECT_EQ(mapEquality(new_ts.map, ps.map), true);
    EXPECT_EQ(new_ts.num_flags, ps.num_flags);
    EXPECT_EQ(flagOffsetsEquality(ps.flag_offsets, ts.flag_offsets), true);

    // default values
    EXPECT_EQ(new_ts.num_towers, ps.num_towers);
    EXPECT_EQ(new_ts.num_enemy_towers, ps.num_enemy_towers);
}
