#include "constants/actor.h"
#include "logger/mocks/logger_mock.h"
#include "state/command_giver.h"
#include "state/map/map.h"
#include "state/mocks/state_mock.h"
#include "state/mocks/state_syncer_mock.h"
#include "state/path_planner/path_planner.h"
#include "state/score_manager/score_manager.h"
#include "gtest/gtest.h"

using namespace state;
using namespace std;
using namespace testing;
using namespace player_state;

class CommandGiverTest : public Test {
  protected:
    size_t map_size;
    Map *map;
    StateMock *state;
    LoggerMock *logger;
    StateSyncerMock *state_syncer;
    unique_ptr<ScoreManager> score_manager;
    unique_ptr<PathPlanner> path_planner;
    unique_ptr<CommandGiver> command_giver;
    array<player_state::State, 2> player_states;
    array<vector<state::Bot *>, 2> state_bots;
    array<vector<state::Tower *>, 2> state_towers;
    vector<DoubleVec2D> bot_positions, tower_positions;

  public:
    void
    manageActorExpectations(array<vector<state::Bot *>, 2> state_bots,
                            array<vector<state::Tower *>, 2> state_towers) {
        EXPECT_CALL(*state, getBots).WillOnce(Return(state_bots));
        EXPECT_CALL(*state, getTowers).WillOnce(Return(state_towers));
    }

    void runCommands(array<player_state::State, 2> &temp_player_states) {
        command_giver->runCommands(temp_player_states, {false, false});
        temp_player_states = player_states;
    }

    CommandGiverTest() {
        // Creating the mock classes
        // TODO: Change to unique_ptr
        state = new StateMock();
        logger = new LoggerMock();
        state_syncer = new StateSyncerMock();
        command_giver = make_unique<CommandGiver>(state, logger);
        array<size_t, 2> scores = {0, 0};
        score_manager = make_unique<ScoreManager>(scores);

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
                player_states[0].map[x][y] = map_element;
                player_states[1].map[map_size - 1 - x][map_size - 1 - y] =
                    map_element;
            }
            test_map.push_back(map_row);
        }

        // Assigning the tower positions and bot positions for later use and
        // reference
        tower_positions = {{0.5, 0.5}, {4.5, 4.5}};
        bot_positions = {{1, 1}, {4, 4}};

        // Assigning the flag locations
        test_map[2][2] = state::TerrainType::FLAG;
        player_states[0].map[2][2].setTerrain(F);
        player_states[1].map[2][2].setTerrain(F);

        // Assigning tower locations
        test_map[tower_positions[0].y][tower_positions[0].x] =
            state::TerrainType::TOWER;
        test_map[tower_positions[1].y][tower_positions[1].x] =
            state::TerrainType::TOWER;
        player_states[0]
            .map[tower_positions[0].y][tower_positions[0].x]
            .setTerrain(T);
        player_states[0]
            .map[tower_positions[1].y][tower_positions[1].x]
            .setTerrain(T);
        player_states[1]
            .map[tower_positions[0].y][tower_positions[0].x]
            .setTerrain(T);
        player_states[1]
            .map[tower_positions[1].y][tower_positions[1].x]
            .setTerrain(T);

        map = new Map(test_map, map_size);
        path_planner = make_unique<PathPlanner>(map);

        // Clearing player state
        for (int player_id = 0; player_id < 2; ++player_id) {
            player_states[player_id].bots.clear();
            player_states[player_id].enemy_bots.clear();
            player_states[player_id].towers.clear();
            player_states[player_id].enemy_towers.clear();
            player_states[player_id].num_bots = 0;
            player_states[player_id].num_enemy_bots = 0;
            player_states[player_id].num_towers = 0;
            player_states[player_id].num_enemy_towers = 0;
        }

        // Creating an incrementing actor id
        size_t actor_id = 1;

        // Creating bots for both players
        for (int player_id = 0;
             player_id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
             ++player_id) {
            int enemy_id =
                (player_id + 1) % static_cast<size_t>(PlayerId::PLAYER_COUNT);

            // Assigning bot positions
            DoubleVec2D position = bot_positions[0];
            DoubleVec2D flipped_position =
                DoubleVec2D(map_size - position.x, map_size - position.y);

            player_states[player_id].num_bots = 1;
            player_states[player_id].num_enemy_bots = 1;

            player_state::Bot player_bot;
            player_bot.id = actor_id++;

            // Assigning player_bot's position for each player and appending
            // it to player's towers
            player_bot.position = position;
            player_states[player_id].bots.push_back(player_bot);

            // Flipping the bot position and appending it to the enemy bots
            // of other player
            player_bot.position = flipped_position;
            player_states[enemy_id].enemy_bots.push_back(player_bot);
        }

        // Creating towers for both players
        for (int player_id = 0;
             player_id < static_cast<int64_t>(PlayerId::PLAYER_COUNT);
             ++player_id) {
            player_states[player_id].num_towers = 1;
            player_states[player_id].num_enemy_towers = 1;

            // Assigning bot positions
            DoubleVec2D position = tower_positions[0];
            DoubleVec2D flipped_position =
                DoubleVec2D(map_size - position.x, map_size - position.y);

            int enemy_id =
                (player_id + 1) % static_cast<size_t>(PlayerId::PLAYER_COUNT);

            player_state::Tower player_tower;
            player_tower.id = actor_id++;

            // Assigning player_tower's position for each player and appending
            // it to player's towers
            player_tower.position = position;

            player_states[player_id].towers.push_back(player_tower);

            // Flipping the bot position and appending it to the enemy bots of
            // other player
            player_tower.position = flipped_position;

            player_states[enemy_id].enemy_towers.push_back(player_tower);
        }

        player_states[0].scores = {1000, 1000};

        player_states[1].scores = {1000, 1000};

        // Creating state bots and towers
        auto state_bot1 =
            new state::Bot(1, PlayerId::PLAYER1, 100, 100, bot_positions[0], 1,
                           1, 1, score_manager.get(), path_planner.get(),
                           BlastCallback{}, ConstructTowerCallback{});

        auto state_bot2 =
            new state::Bot(2, PlayerId::PLAYER2, 100, 100, bot_positions[1], 1,
                           1, 1, score_manager.get(), path_planner.get(),
                           BlastCallback{}, ConstructTowerCallback{});

        auto state_tower1 =
            new state::Tower(3, PlayerId::PLAYER1, 100, 100, tower_positions[0],
                             2, 2, score_manager.get(), BlastCallback{});
        auto state_tower2 =
            new state::Tower(4, PlayerId::PLAYER2, 100, 100, tower_positions[1],
                             2, 2, score_manager.get(), BlastCallback{});

        state_bots = {{{state_bot1}, {state_bot2}}};
        state_towers = {{{state_tower1}, {state_tower2}}};
    }

    ~CommandGiverTest() {
        delete state;
        delete state_syncer;
        delete logger;
    }
};

TEST_F(CommandGiverTest, AlterActorProperties) {
    // Creating a temporary player state to modify
    array<player_state::State, 2> temp_player_states = player_states;

    // Returning the map repeatedly
    EXPECT_CALL(*state, getMap).WillRepeatedly(Return(map));

    // ------------- Alter bot properties -----------
    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_BOT_PROPERTY, _));
    temp_player_states[0].bots[0].id = -1;
    runCommands(temp_player_states);

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER2,
                                  ErrorType::NO_ALTER_BOT_PROPERTY, _));
    temp_player_states[1].bots[0].hp = -1;
    runCommands(temp_player_states);

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_BOT_PROPERTY, _));
    temp_player_states[0].bots[0].position = DoubleVec2D(200, 200);
    runCommands(temp_player_states);

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_BOT_PROPERTY, _));
    temp_player_states[0].bots[0].state = player_state::BotState::TRANSFORM;
    runCommands(temp_player_states);

    // ------------- TOWER TESTS ------------------

    // Alter tower properties
    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_TOWER_PROPERTY, _));
    temp_player_states[0].towers[0].hp = -1;
    runCommands(temp_player_states);

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_TOWER_PROPERTY, _));
    temp_player_states[0].towers[0].id = -1;
    runCommands(temp_player_states);

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER2,
                                  ErrorType::NO_ALTER_TOWER_PROPERTY, _));
    temp_player_states[1].towers[0].position = DoubleVec2D(12, 25);
    runCommands(temp_player_states);

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER2,
                                  ErrorType::NO_ALTER_TOWER_PROPERTY, _));
    temp_player_states[1].towers[0].state = player_state::TowerState::BLAST;
    runCommands(temp_player_states);
}

TEST_F(CommandGiverTest, MultipleBotTasks) {
    // Creating a temporary player state to modify
    array<player_state::State, 2> temp_player_states = player_states;

    // Returning the map repeatedly
    EXPECT_CALL(*state, getMap).WillRepeatedly(Return(map));

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].destination = DoubleVec2D(0, 0);
    temp_player_states[0].bots[0].blasting = true;
    runCommands(temp_player_states);

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].blasting = true;
    temp_player_states[0].bots[0].transform_destination = DoubleVec2D(0, 0);
    runCommands(temp_player_states);

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].transforming = true;
    temp_player_states[0].bots[0].final_destination = DoubleVec2D(0, 0);
    runCommands(temp_player_states);

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].final_destination = DoubleVec2D(0, 0);
    temp_player_states[0].bots[0].transform_destination = DoubleVec2D(0, 0);
    runCommands(temp_player_states);
}

TEST_F(CommandGiverTest, InvalidPositionTests) {
    // Creating a temporary player state to modify
    array<player_state::State, 2> temp_player_states = player_states;

    // Returning the map repeatedly
    EXPECT_CALL(*state, getMap).WillRepeatedly(Return(map));

    // Trying to move bot to an invalid position
    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::INVALID_MOVE_POSITION, _));
    temp_player_states[0].bots[0].destination = DoubleVec2D(-10, -5);
    runCommands(temp_player_states);

    // Trying to blast a bot in an invalid position
    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::INVALID_BLAST_POSITION, _));
    temp_player_states[0].bots[0].final_destination = DoubleVec2D(-10, -5);
    runCommands(temp_player_states);

    // Trying to transform a bot in an invalid position
    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::INVALID_TRANSFORM_POSITION, _));
    temp_player_states[0].bots[0].transform_destination = DoubleVec2D(-10, -5);
    runCommands(temp_player_states);

    // // Trying to build tower at invalid position
    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::INVALID_MOVE_POSITION, _));
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER2,
                                  ErrorType::INVALID_BLAST_POSITION, _));

    temp_player_states[0].bots[0].destination = DoubleVec2D(5, 5);
    temp_player_states[1].bots[0].final_destination = DoubleVec2D(0, 0);
    runCommands(temp_player_states);
}

TEST_F(CommandGiverTest, ExceedTowerLimit) {
    // Creating a temporary player state to modify
    array<player_state::State, 2> temp_player_states = player_states;

    // Returning the map repeatedly
    EXPECT_CALL(*state, getMap).WillRepeatedly(Return(map));

    // Exceed tower limit
    for (size_t tower_count = 0;
         tower_count < Constants::Actor::MAX_NUM_TOWERS - 1; ++tower_count) {
        size_t actor_id = Actor::getNextActorId();
        auto state_tower = new state::Tower(
            actor_id, PlayerId::PLAYER1, 100, 100, DoubleVec2D(0, 0), 2, 2,
            score_manager.get(), BlastCallback{});
        state_towers[0].push_back(state_tower);

        // Creating the player state tower
        player_state::Tower player_tower;
        player_tower.id = actor_id;
        player_tower.hp = 100;
        player_tower.position = DoubleVec2D(0, 0);
        player_tower.reset();

        // Inserting tower into player state
        temp_player_states[0].towers.push_back(player_tower);

        // Flipping position and appending to enemy towers
        player_tower.position = DoubleVec2D(4, 4);
        temp_player_states[1].enemy_towers.push_back(player_tower);
    }

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger,
                logError(PlayerId::PLAYER1, ErrorType::TOWER_LIMIT_REACHED, _));
    temp_player_states[0].bots[0].transforming = true;
    runCommands(temp_player_states);
}

// Additional actors are added into the player state and checks are done to
// see if errors are triggered
TEST_F(CommandGiverTest, AddAndRemoveActors) {
    // Creating a temporary player state to modify
    array<player_state::State, 2> temp_player_states = player_states;

    // Returning the map repeatedly
    EXPECT_CALL(*state, getMap).WillRepeatedly(Return(map));

    auto extra_bot = player_state::Bot();
    auto extra_tower = player_state::Tower();

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NUMBER_OF_BOTS_MISMATCH, _));
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER2,
                                  ErrorType::NUMBER_OF_TOWERS_MISMATCH, _));
    temp_player_states[0].bots.push_back(extra_bot);
    temp_player_states[1].enemy_towers.push_back(extra_tower);
    runCommands(temp_player_states);
}

TEST_F(CommandGiverTest, EarlyBlastTower) {
    // Creating a temporary player state to modify
    array<player_state::State, 2> temp_player_states = player_states;

    // Returning the map repeatedly
    EXPECT_CALL(*state, getMap).WillRepeatedly(Return(map));

    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::NO_EARLY_BLAST_TOWER, _));

    // Trying to make tower blast prematurely
    temp_player_states[0].towers[0].blasting = true;
    runCommands(temp_player_states);
}

TEST_F(CommandGiverTest, TransformBasePosition) {
    // Creating a temporary player state to modify
    array<player_state::State, 2> temp_player_states = player_states;

    // Returning the map repeatedly
    EXPECT_CALL(*state, getMap).WillRepeatedly(Return(map));

    // Trying to make a bot move to transform into a spawn position
    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER1,
                                  ErrorType::INVALID_TRANSFORM_POSITION, _));

    temp_player_states[0].bots[0].transform_destination = DoubleVec2D(0.5, 0.5);
    runCommands(temp_player_states);

    // Trying to make a bot transform in a spawn position
    manageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, logError(PlayerId::PLAYER2,
                                  ErrorType::INVALID_TRANSFORM_POSITION, _));

    temp_player_states[1].bots[0].transform_destination = DoubleVec2D(4.5, 4.5);
    runCommands(temp_player_states);
}
