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
        ts.bots[5].blast();
        ts.bots[6].transform();
        ts.num_enemy_bots = 20;
        ts.enemy_bots[10].blast();
        ts.enemy_bots[11].transform();
        ts.num_towers = 30;
        ts.towers[29].blast();
        ts.scores = {1000, 2000};

        ps.num_bots = 50;
        ps.bots.assign(ps.num_bots, player_state::Bot());
        ps.bots[10].blast();
        ps.bots[20].transform();
        ps.num_enemy_bots = 30;
        ps.enemy_bots.assign(ps.num_enemy_bots, player_state::Bot());
        ps.enemy_bots[7].blast();
        ps.enemy_bots[9].transform();
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

    template <typename T, size_t _>
    bool compareVectorAndArray(const vector<T> &vec, const array<T, _> &arr) {
        return all_of(
            vec.begin(), vec.end(),
            [&arr, i = 0](const T &val) mutable { return val == arr[i++]; });
    }
};

TEST_F(TransferStateTest, ConvertToPlayerStateTest) {
    auto new_ps = ConvertToPlayerState(ts);

    // non default values, changed in TransferStateTest constructor
    EXPECT_EQ(new_ps.num_bots, ts.num_bots); // must equal 10
    EXPECT_EQ(compareVectorAndArray(new_ps.bots, ts.bots), true);

    EXPECT_EQ(new_ps.num_enemy_bots, ts.num_enemy_bots); // must equal 20
    EXPECT_EQ(compareVectorAndArray(new_ps.enemy_bots, ts.enemy_bots), true);

    EXPECT_EQ(new_ps.num_towers, ts.num_towers); // must equal 30
    EXPECT_EQ(compareVectorAndArray(new_ps.towers, ts.towers), true);

    EXPECT_EQ(pairFromScores(new_ps.scores), pairFromScores(ts.scores));

    EXPECT_EQ(mapEquality(new_ps.map, ts.map), true);

    EXPECT_EQ(new_ps.num_flags, ts.num_flags);
    EXPECT_EQ(compareVectorAndArray(new_ps.flag_offsets, ts.flag_offsets),
              true);

    // default values
    EXPECT_EQ(new_ps.num_enemy_towers, ts.num_enemy_towers);
    EXPECT_EQ(compareVectorAndArray(new_ps.enemy_towers, ts.enemy_towers),
              true);
}

TEST_F(TransferStateTest, ConvertToTransferStateTest) {
    auto new_ts = ConvertToTransferState(ps);

    // non default values, changed in TransferStateTest constructor
    EXPECT_EQ(new_ts.num_bots, ps.num_bots); // equals 50
    EXPECT_EQ(compareVectorAndArray(ps.bots, new_ts.bots), true);

    EXPECT_EQ(new_ts.num_enemy_bots, ps.num_enemy_bots); // equals 30
    EXPECT_EQ(compareVectorAndArray(ps.enemy_bots, new_ts.enemy_bots), true);

    EXPECT_EQ(pairFromScores(new_ts.scores), pairFromScores(ps.scores));

    EXPECT_EQ(mapEquality(new_ts.map, ps.map), true);

    EXPECT_EQ(new_ts.num_flags, ps.num_flags);
    EXPECT_EQ(compareVectorAndArray(ps.flag_offsets, ts.flag_offsets), true);

    // default values
    EXPECT_EQ(new_ts.num_towers, ps.num_towers);
    EXPECT_EQ(compareVectorAndArray(ps.towers, new_ts.towers), true);
    EXPECT_EQ(new_ts.num_enemy_towers, ps.num_enemy_towers);
    EXPECT_EQ(compareVectorAndArray(ps.enemy_towers, new_ts.enemy_towers),
              true);
}
