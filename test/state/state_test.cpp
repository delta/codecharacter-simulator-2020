#include "state/state.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;
using namespace state;

const auto L = state::TerrainType::LAND;
const auto W = state::TerrainType::WATER;
const auto F = state::TerrainType::FLAG;
const auto T = state::TerrainType::TOWER;

class StateTest : public Test {
    public:

    Map *map;
    ScoreManager* score_manager;
    unique_ptr<PathPlanner> path_planner;
    unique_ptr<State> state;
    array<vector<Bot *>, 2> state_bots;
    array<vector<Tower *>, 2> state_towers;
    size_t map_size;

    StateTest() {
        map_size = 5;

        auto u_score_manager = make_unique<ScoreManager>();
        score_manager = u_score_manager.get();

        // clang-format off
        auto test_map = vector<vector<TerrainType>>{
            {W, L, L, L, T}, 
            {L, L, L, L, L},
            {L, L, L, L, L},
            {L, L, L, L, L},
            {T, L, L, L, W},
        };
        // clang-format on

        auto u_map = make_unique<Map>(test_map, map_size);
        map = u_map.get();
        path_planner = make_unique<PathPlanner>(std::move(u_map));

        // Creating bots and towers
        int64_t bot_hp = 100, bot_max_hp = 100, speed = 5, bot_blast_range = 3,
                bot_damage_points = 30;
        auto bot1 = new Bot(
            PlayerId::PLAYER1, bot_hp, bot_max_hp, DoubleVec2D(1, 1), speed,
            bot_blast_range, bot_damage_points, score_manager,
            path_planner.get(), BlastCallback{}, ConstructTowerCallback{});
        auto bot2 = new Bot(
            PlayerId::PLAYER2, bot_hp, bot_max_hp, DoubleVec2D(4, 4), speed,
            bot_blast_range, bot_damage_points, score_manager,
            path_planner.get(), BlastCallback{}, ConstructTowerCallback{});
        state_bots = {{{bot1}, {bot2}}};
        int64_t tower_hp = 250, tower_max_hp = 250, tower_blast_range = 5,
                tower_damage_points = 50;
        auto tower1 =
            new Tower(PlayerId::PLAYER1, tower_hp, tower_max_hp,
                               {0, 0}, tower_damage_points, tower_blast_range,
                               score_manager, BlastCallback{});
        auto tower2 =
            new Tower(PlayerId::PLAYER2, tower_hp, tower_max_hp,
                               {4, 4}, tower_damage_points, tower_blast_range,
                               score_manager, BlastCallback{});
        state_towers = {{ {tower1}, {tower2}}};

        // state = make_unique<State>(std::move(u_move))                               
    }
};

TEST_F(StateTest, MoveBotTest) {

}
