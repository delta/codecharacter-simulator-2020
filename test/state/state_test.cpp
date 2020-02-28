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
    unique_ptr<State> state;
    size_t map_size;
    int64_t bot_max_hp, tower_max_hp;

    StateTest() {

        // std::cerr << "Starting state tests\n\n\n\n\n\n\n\n";
        map_size = 5;

        auto score_manager = make_unique<ScoreManager>();

        // clang-format off
        auto test_map = vector<vector<TerrainType>>{
            {T, L, L, L, W}, 
            {L, L, L, L, L},
            {L, L, L, L, L},
            {L, L, L, F, L},
            {W, L, L, L, T},
        };
        // clang-format on

        auto map = make_unique<Map>(test_map, map_size);
        auto path_planner = make_unique<PathPlanner>(map.get());

        // Creating bots and towers
        int64_t bot_hp = 100, speed = 5, bot_blast_range = 3,
                bot_damage_points = 30;
        bot_max_hp = 100;

        auto state_bots = array<vector<unique_ptr<Bot>>, 2>{};
        auto state_towers = array<vector<unique_ptr<Tower>>, 2>{};

        state_bots[0].push_back(make_unique<Bot>(
            1, PlayerId::PLAYER1, bot_hp, bot_max_hp, DoubleVec2D(1, 1), speed,
            bot_blast_range, bot_damage_points, score_manager.get(),
            path_planner.get(), BlastCallback{}, ConstructTowerCallback{}));
        state_bots[1].push_back(make_unique<Bot>(
            2, PlayerId::PLAYER2, bot_hp, bot_max_hp, DoubleVec2D(4, 4), speed,
            bot_blast_range, bot_damage_points, score_manager.get(),
            path_planner.get(), BlastCallback{}, ConstructTowerCallback{}));

        int64_t tower_hp = 250, tower_blast_range = 5, tower_damage_points = 50;
        tower_max_hp = 250;

        state_towers[0].push_back(make_unique<Tower>(
            PlayerId::PLAYER1, tower_hp, tower_max_hp, DoubleVec2D(0, 0),
            tower_damage_points, tower_blast_range, score_manager.get(),
            BlastCallback{}));
        state_towers[1].push_back(make_unique<Tower>(
            PlayerId::PLAYER2, tower_hp, tower_max_hp, DoubleVec2D(4, 4),
            tower_damage_points, tower_blast_range, score_manager.get(),
            BlastCallback{}));

        state = make_unique<State>(move(map), move(score_manager),
                                   move(path_planner), move(state_bots),
                                   move(state_towers));
    }
};

TEST_F(StateTest, SamePointers) {
    auto state_bots = state->getBots();
    auto bot = state_bots[0][0];
    auto path_planner1 = state->getPathPlanner();
    auto path_planner2 = bot->getPathPlanner();
    ASSERT_EQ(path_planner1, path_planner2);
}

TEST_F(StateTest, GetActors) {
    auto bots = state->getBots();
    auto towers = state->getTowers();

    ASSERT_EQ(bots[0].size(), 1);
    ASSERT_EQ(bots[1].size(), 1);
    ASSERT_EQ(towers[0].size(), 1);
    ASSERT_EQ(towers[1].size(), 1);
}

TEST_F(StateTest, GetMap) {
    auto map = state->getMap();
    ASSERT_EQ(!map, false);

    size_t map_size = map->getSize();
    ASSERT_EQ(map_size, 5);
    size_t p1 = 0, p2 = 3;
    ASSERT_EQ(map->getTerrainType(p1, p1), TerrainType::TOWER);
    ASSERT_EQ(map->getTerrainType(p2, p2), TerrainType::FLAG);
}

TEST_F(StateTest, MoveBotTest) {
    auto bots = state->getBots();
    auto bot = bots[0][0];
    ASSERT_EQ(bot->isDestinationSet(), false);

    // Calling move bot in state and checking if bots destination is set
    state->moveBot(1, DoubleVec2D(2, 2));

    // Getting PLAYER1's first bot and checking if his destination is set
    DoubleVec2D bot_destination = bot->getDestination();
    ASSERT_EQ(bot->isDestinationSet(), true);
    ASSERT_EQ(bot_destination, DoubleVec2D(2, 2));
}

TEST_F(StateTest, CreateTowerTest) {
    // Transforming the first bot of PLAYER2 into a tower
    auto bots = state->getBots();
    auto bot = bots[1][0];
    auto score_manager = state->getScoreManager();

    int64_t bot_id = bot->getActorId();
    int64_t hp = 50;
    bot->setHp(50);

    ASSERT_EQ(bots[0].size(), 1);
    ASSERT_EQ(bots[1].size(), 1);

    bot->setDestination(DoubleVec2D(3.5, 3.5));
    bot->update();
    bot->lateUpdate();

    // Checking initial number of bots on flags
    auto bot_counts = score_manager->getBotCounts();
    ASSERT_EQ(bot_counts[1], 1);

    // Calling create tower on this bot
    state->createTower(bot);

    bots = state->getBots();
    auto towers = state->getTowers();

    // Checking to see if the bot was removed from state and if a tower was added to state
    ASSERT_EQ(bots[1].size(), 1);
    ASSERT_EQ(towers[1].size(), 2);

    // Checking to see if the tower has the same id as the bot
    auto tower = towers[1][1];
    ASSERT_EQ(tower->getActorId(), bot_id);

    // Check if the tower HP has been scaled correctly
    double hp_ratio = double(tower_max_hp) / double(bot_max_hp);
    
    int64_t tower_hp = hp * hp_ratio;
    ASSERT_EQ(tower->getHp(), tower_hp);

    // Checking score_manager counts because the bot was on a FLAG
    bot_counts = score_manager->getBotCounts();
    auto tower_counts = score_manager->getTowerCounts();
    ASSERT_EQ(tower_counts[1], 1);
    ASSERT_EQ(bot_counts[1], 0);
}
