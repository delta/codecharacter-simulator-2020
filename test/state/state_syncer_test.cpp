#include "logger/mocks/logger_mock.h"
#include "state/mocks/command_giver_mock.h"
#include "state/mocks/state_mock.h"
#include "state/path_planner/path_planner.h"
#include "state/state_syncer.h"
#include "gtest/gtest.h"

using namespace std;
using namespace testing;
using namespace state;

class StateSyncerTest : public Test {
  public:
    Map *map;
    unique_ptr<StateSyncer> state_syncer;
    unique_ptr<PathPlanner> path_planner;
    StateMock *state;
    LoggerMock *logger;
    CommandGiverMock *command_giver;
    array<player_state::State, 2> player_states;
    size_t map_size;

    // Storing state's variables
    vector<DoubleVec2D> bot_positions;
    vector<DoubleVec2D> tower_positions;
    array<vector<state::Bot *>, 2> state_bots;
    array<vector<state::Tower *>, 2> state_towers;

    void manageStateExpectations(array<vector<state::Bot *>, 2> bots,
                                 array<vector<state::Tower *>, 2> towers) {
        EXPECT_CALL(*this->state, getBots)
            .Times(5)
            .WillRepeatedly(Return(bots));
        EXPECT_CALL(*this->state, getTowers)
            .Times(5)
            .WillRepeatedly(Return(towers));
        EXPECT_CALL(*this->state, getMap).Times(9).WillRepeatedly(Return(map));
        EXPECT_CALL(*this->logger, LogState);
    }

    StateSyncerTest() {
        // Creating the necessary properties of state syncer
        auto u_state = make_unique<StateMock>();
        auto u_logger = make_unique<LoggerMock>();
        auto u_command_giver = make_unique<CommandGiverMock>();

        state = u_state.get();
        logger = u_logger.get();
        command_giver = u_command_giver.get();

        state_syncer = make_unique<StateSyncer>(std::move(u_state),
                                                std::move(u_command_giver),
                                                std::move(u_logger));

        // Creating the player and state map
        map_size = 5;

        /**
         * Initialize the player match
         * L - Land
         * W - Water
         * F - Flag
         * T - Tower
         *
         * Map Diagram : (Player 1 perspective)
         *
         * W L L L T
         * L W L L L
         * L L F L L
         * L L L W L
         * T L L L W
         *
         */

        const auto L = player_state::TerrainType::LAND;
        const auto W = player_state::TerrainType::WATER;
        const auto F = player_state::TerrainType::FLAG;
        const auto T = player_state::TerrainType::TOWER;

        // Creating the player state map
        vector<vector<state::TerrainType>> test_map;
        for (size_t x = 0; x < map_size; ++x) {
            vector<state::TerrainType> map_row;
            MapElement map_element{};
            for (size_t y = 0; y < map_size; ++y) {
                if (x + y == map_size - 1) {
                    map_element.setTerrain(W);
                    map_row.push_back(state::TerrainType::WATER);
                } else {
                    map_element.setTerrain(L);
                    map_row.push_back(state::TerrainType::LAND);
                }
            }
            test_map.push_back(map_row);
        }

        // Assinging the tower positions and bot positions for later use and
        // reference
        tower_positions = {{0, 0}, {4, 4}};
        bot_positions = {{1, 1}, {4, 4}};

        // Assinging the flag locations
        test_map[2][2] = state::TerrainType::FLAG;

        // Assinging tower locations
        test_map[tower_positions[0].y][tower_positions[0].x] =
            state::TerrainType::TOWER;
        test_map[tower_positions[1].y][tower_positions[1].x] =
            state::TerrainType::TOWER;

        auto map_u = make_unique<Map>(test_map, map_size);
        map = map_u.get();
        path_planner = make_unique<PathPlanner>(std::move(map_u));

        // Creating state bots and towers
        auto state_bot1 = new state::Bot(
            0, PlayerId::PLAYER1, 100, 100, bot_positions[0], 1, 1, 1,
            path_planner.get(), BlastCallback{}, ConstructTowerCallback{});

        auto state_bot2 = new state::Bot(
            1, PlayerId::PLAYER2, 100, 100, bot_positions[1], 1, 1, 1,
            path_planner.get(), BlastCallback{}, ConstructTowerCallback{});

        auto state_tower1 =
            new state::Tower(2, PlayerId::PLAYER1, 100, 100, tower_positions[0],
                             2, 2, BlastCallback{});
        auto state_tower2 =
            new state::Tower(3, PlayerId::PLAYER2, 100, 100, tower_positions[1],
                             2, 2, BlastCallback{});

        state_bots = {{{state_bot1}, {state_bot2}}};
        state_towers = {{{state_tower1}, {state_tower2}}};
    }
};

TEST_F(StateSyncerTest, updatePlayerStates) {
    // Adding new bots to player state to check if they are added to the player
    // state
    auto new_bots2 = state_bots;
    auto bot2 = new state::Bot(4, PlayerId::PLAYER1, 100, 100,
                               DoubleVec2D(3, 3), 1, 1, 1, path_planner.get(),
                               BlastCallback{}, ConstructTowerCallback{});
    new_bots2[0].push_back(bot2);

    // Expecting command taker calls
    manageStateExpectations(new_bots2, state_towers);
    this->state_syncer->updatePlayerStates(player_states);

    ASSERT_EQ(player_states[0].bots.size(), 2);
    ASSERT_EQ(player_states[0].num_bots, 2);
    ASSERT_EQ(player_states[1].enemy_bots.size(), 2);
    ASSERT_EQ(player_states[1].num_enemy_bots, 2);

    // Erasing a bot to check removal of bot
    auto new_bots3 = new_bots2;
    player_states = player_states;
    new_bots3[1].erase(new_bots3[1].begin(), new_bots3[1].begin() + 1);

    // Expecting command taker calls
    manageStateExpectations(new_bots3, state_towers);
    this->state_syncer->updatePlayerStates(player_states);

    ASSERT_EQ(player_states[1].bots.size(), 0);
    ASSERT_EQ(player_states[1].num_bots, 0);
    ASSERT_EQ(player_states[0].enemy_bots.size(), 0);
    ASSERT_EQ(player_states[0].num_enemy_bots, 0);
    ASSERT_EQ(player_states[0].bots.size(), 2);
    ASSERT_EQ(player_states[0].num_bots, 2);
    ASSERT_EQ(player_states[1].enemy_bots.size(), 2);
    ASSERT_EQ(player_states[1].num_enemy_bots, 2);
    ASSERT_EQ(player_states[0].bots[1].position, DoubleVec2D(3, 3));

    // Adding a new tower
    auto new_towers2 = state_towers;
    auto state_tower2 =
        new state::Tower(5, PlayerId::PLAYER2, 100, 100, DoubleVec2D(4, 3), 2,
                         2, BlastCallback{});
    new_towers2[1].push_back(state_tower2);

    // Checking for bots and tower's new states
    manageStateExpectations(state_bots, new_towers2);
    this->state_syncer->updatePlayerStates(player_states);

    ASSERT_EQ(player_states[1].towers.size(), 2);
    ASSERT_EQ(player_states[1].num_towers, 2);
    ASSERT_EQ(player_states[0].enemy_towers.size(), 2);
    ASSERT_EQ(player_states[0].num_enemy_towers, 2);
    ASSERT_EQ(player_states[1].towers[1].position, DoubleVec2D(0, 1));
    ASSERT_EQ(player_states[0].enemy_towers[1].position, DoubleVec2D(4, 3));

    // Checking for bots and tower's new states
    manageStateExpectations(state_bots, state_towers);
    this->state_syncer->updatePlayerStates(player_states);

    // Checking if the map is assigned properly
    const auto L = player_state::TerrainType::LAND;
    const auto W = player_state::TerrainType::WATER;
    const auto F = player_state::TerrainType::FLAG;
    const auto T = player_state::TerrainType::TOWER;

    ASSERT_EQ(player_states[0].map[2][2].getTerrain(), F);
    ASSERT_EQ(player_states[1].map[2][2].getTerrain(), F);

    ASSERT_EQ(player_states[0].map[0][0].getTerrain(), T);
    ASSERT_EQ(player_states[1].map[4][4].getTerrain(), T);

    ASSERT_EQ(player_states[0].map[0][4].getTerrain(), W);
    ASSERT_EQ(player_states[1].map[4][0].getTerrain(), W);

    ASSERT_EQ(player_states[0].map[4][4].getTerrain(), T);
    ASSERT_EQ(player_states[1].map[0][0].getTerrain(), T);

    ASSERT_EQ(player_states[0].map[0][2].getTerrain(), L);
    ASSERT_EQ(player_states[1].map[4][2].getTerrain(), L);

    // Checking bots and towers
    ASSERT_EQ(player_states[0].bots[0].id, 0);
    ASSERT_EQ(player_states[0].bots[0].hp, 100);
    ASSERT_EQ(player_states[0].bots[0].position, DoubleVec2D(1, 1));
    ASSERT_EQ(player_states[1].enemy_bots[0].position, DoubleVec2D(4, 4));
    ASSERT_EQ(player_states[0].bots[0].state, player_state::BotState::IDLE);
    ASSERT_EQ(player_states[1].enemy_bots[0].id, 0);

    ASSERT_EQ(player_states[1].bots[0].id, 1);
    ASSERT_EQ(player_states[1].bots[0].hp, 100);
    ASSERT_EQ(player_states[1].bots[0].position, DoubleVec2D(1, 1));
    ASSERT_EQ(player_states[1].bots[0].state, player_state::BotState::IDLE);

    ASSERT_EQ(player_states[0].towers[0].id, 2);
    ASSERT_EQ(player_states[0].towers[0].hp, 100);
    ASSERT_EQ(player_states[0].towers[0].position, DoubleVec2D(0, 0));
    ASSERT_EQ(player_states[0].towers[0].state, player_state::TowerState::IDLE);

    ASSERT_EQ(player_states[1].towers[0].id, 3);
    ASSERT_EQ(player_states[1].towers[0].hp, 100);
    ASSERT_EQ(player_states[1].towers[0].position, DoubleVec2D(0, 0));
    ASSERT_EQ(player_states[1].towers[0].state, player_state::TowerState::IDLE);
}
