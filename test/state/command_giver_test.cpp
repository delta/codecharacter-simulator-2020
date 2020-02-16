#include "constants/actor.h"
#include "logger/mocks/logger_mock.h"
#include "state/command_giver.h"
#include "state/map/map.h"
#include "state/mocks/state_mock.h"
#include "state/mocks/state_syncer_mock.h"
#include "state/score_manager/score_manager.h"
#include "gtest/gtest.h"

using namespace state;
using namespace std;
using namespace testing;
using namespace player_state;

class CommandGiverTest : public Test {
  protected:
    size_t map_size;
    unique_ptr<Map> map;
    StateMock *state;
    LoggerMock *logger;
    StateSyncerMock *state_syncer;
    unique_ptr<CommandGiver> command_giver;
    array<player_state::State, 2> player_states;
    array<vector<state::Bot *>, 2> state_bots;
    array<vector<state::Tower *>, 2> state_towers;

  public:
    void
    ManageActorExpectations(array<vector<state::Bot *>, 2> state_bots,
                            array<vector<state::Tower *>, 2> state_towers) {
        EXPECT_CALL(*state, getBots).WillOnce(Return(state_bots));
        EXPECT_CALL(*state, getTowers).WillOnce(Return(state_towers));
    }

    void RunCommands(array<player_state::State, 2> &temp_player_states) {
        command_giver->runCommands(temp_player_states, {false, false});
        temp_player_states = player_states;
    }

    CommandGiverTest() {
        // Creating the mock classes
        auto u_state = make_unique<StateMock>();
        auto u_logger = make_unique<LoggerMock>();
        auto u_state_syncer = make_unique<StateSyncerMock>();
        state = u_state.get();
        logger = u_logger.get();
        state_syncer = u_state_syncer.get();
        command_giver =
            make_unique<CommandGiver>(move(u_state), move(u_logger));

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
         * W L L L L
         * L W L L L
         * L L F L L
         * L L L W L
         * L L L L W
         *
         */

        const auto L = player_state::TerrainType::LAND;
        const auto W = player_state::TerrainType::WATER;
        const auto F = player_state::TerrainType::FLAG;
        const auto T = player_state::TerrainType::TOWER;

        // Creating the player state map
        vector<vector<state::TerrainType>> dummy_map;
        for (int x = 0; x < map_size; ++x) {
            vector<state::TerrainType> map_row;
            MapElement ele;
            for (int y = 0; y < map_size; ++y) {
                if (x + y == map_size - 1) {
                    ele.setTerrain(W);
                    map_row.push_back(state::TerrainType::WATER);
                } else {
                    ele.setTerrain(L);
                    map_row.push_back(state::TerrainType::LAND);
                }
                player_states[0].map[x][y] = ele;
                player_states[1].map[map_size - 1 - x][map_size - 1 - y] = ele;
            }
            dummy_map.push_back(map_row);
        }

        // Assinging the flag locations
        dummy_map[2][2] = state::TerrainType::FLAG;
        player_states[0].map[2][2].setTerrain(F);
        player_states[1].map[2][2].setTerrain(F);

        map = make_unique<Map>(dummy_map, map_size);

        // Creating an incrementing actor id
        size_t actor_id = 0;

        // Creating bots for both players
        for (int player_id = 0; player_id < 2; ++player_id) {
            player_states[player_id].bots.clear();
            player_states[player_id].enemy_bots.clear();
            int enemy_id =
                (player_id + 1) % static_cast<size_t>(PlayerId::PLAYER_COUNT);

            // Assinging bot positions
            DoubleVec2D position = DoubleVec2D(1, 1),
                        flipped_position =
                            DoubleVec2D(map_size - 1, map_size - 1);

            // Flipping the positions for player 2
            if (player_id == 1) {
                swap(position, flipped_position);
            }

            for (int num_bots = 0; num_bots < 1; ++num_bots) {
                player_state::Bot player_bot;
                player_bot.id = actor_id++;

                // Assiging player_bot's position for each player and appending
                // it to player's towers
                player_bot.position = position;
                player_states[player_id].bots.push_back(player_bot);

                // Flipping the bot position and appending it to the enemy bots
                // of other player
                player_bot.position = flipped_position;
                player_states[enemy_id].enemy_bots.push_back(player_bot);
            }
        }

        // Creating towers for both players
        for (int player_id = 0; player_id < 2; ++player_id) {

            // Assinging bot positions
            DoubleVec2D position = DoubleVec2D(0, 0),
                        flipped_position =
                            DoubleVec2D(map_size - 1, map_size - 1);

            // Flipping the positions for player 2
            if (player_id == 1) {
                swap(position, flipped_position);
            }

            int enemy_id =
                (player_id + 1) % static_cast<size_t>(PlayerId::PLAYER_COUNT);

            player_states[player_id].towers.clear();
            player_states[player_id].enemy_towers.clear();

            player_state::Tower player_tower;
            player_tower.id = actor_id++;

            // Assiging player_tower's position for each player and appending it
            // to player's towers
            player_tower.position = position;

            player_states[player_id].towers.push_back(player_tower);

            // Flipping the bot position and appending it to the enemy bots of
            // other player
            player_tower.position = flipped_position;

            player_states[enemy_id].enemy_towers.push_back(player_tower);
        }

        player_states[0].score = 1000;
        player_states[1].score = 1000;

        // Creating state bots and towers
        auto state_bot1 = new state::Bot(
            Actor::getNextActorId(), PlayerId::PLAYER1, ActorType::BOT, 100,
            100, DoubleVec2D(1, 1), 1, 1, 1, BlastCallback{});

        auto state_bot2 = new state::Bot(
            Actor::getNextActorId(), PlayerId::PLAYER2, ActorType::BOT, 100,
            100, DoubleVec2D(map_size - 1, map_size - 1), 1, 1, 1,
            BlastCallback{});

        auto state_tower1 = new state::Tower(
            Actor::getNextActorId(), PlayerId::PLAYER1, ActorType::TOWER, 100,
            100, DoubleVec2D(0, 0), 2, 2, BlastCallback{});
        auto state_tower2 = new state::Tower(
            Actor::getNextActorId(), PlayerId::PLAYER2, ActorType::TOWER, 100,
            100, DoubleVec2D(map_size - 1, map_size - 1), 2, 2,
            BlastCallback{});

        state_bots = {{{state_bot1}, {state_bot2}}};
        state_towers = {{{state_tower1}, {state_tower2}}};
    }
};

TEST_F(CommandGiverTest, CommandExecutionTest) {
    // Creating a temporary player state to modify
    array<player_state::State, 2> temp_player_states = player_states;

    // Returning the map repeatedly
    EXPECT_CALL(*state, getMap).WillRepeatedly(Return(map.get()));

    // ------------- BOT TESTS ------------------

    // ------------- Alter bot properties -----------
    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_BOT_PROPERTY, _));
    temp_player_states[0].bots[0].id = -1;
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER2,
                                  ErrorType::NO_ALTER_BOT_PROPERTY, _));
    temp_player_states[1].bots[0].hp = -1;
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_BOT_PROPERTY, _));
    temp_player_states[0].bots[0].position = DoubleVec2D(200, 200);
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_BOT_PROPERTY, _));
    temp_player_states[0].bots[0].state = player_state::BotState::TRANSFORM;
    RunCommands(temp_player_states);

    // -------------- Multiple bot tasks ----------------------

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].destination = DoubleVec2D(0, 0);
    temp_player_states[0].bots[0].blasting = true;
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].destination = DoubleVec2D(0, 0);
    temp_player_states[0].bots[0].final_destination = DoubleVec2D(0, 0);
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].destination = DoubleVec2D(0, 0);
    temp_player_states[0].bots[0].transform_destination = DoubleVec2D(0, 0);
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].destination = DoubleVec2D(0, 0);
    temp_player_states[0].bots[0].transforming = true;
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].blasting = true;
    temp_player_states[0].bots[0].transform_destination = DoubleVec2D(0, 0);
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].blasting = true;
    temp_player_states[0].bots[0].final_destination = DoubleVec2D(0, 0);
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER2,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[1].bots[0].blasting = true;
    temp_player_states[1].bots[0].transforming = true;
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].transforming = true;
    temp_player_states[0].bots[0].final_destination = DoubleVec2D(0, 0);
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].transforming = true;
    temp_player_states[0].bots[0].transform_destination = DoubleVec2D(0, 0);
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_MULTIPLE_BOT_TASK, _));
    temp_player_states[0].bots[0].final_destination = DoubleVec2D(0, 0);
    temp_player_states[0].bots[0].transform_destination = DoubleVec2D(0, 0);
    RunCommands(temp_player_states);

    // Trying to move bot to an invalid position
    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::INVALID_MOVE_POSITION, _));
    temp_player_states[0].bots[0].destination = DoubleVec2D(-10, -5);
    RunCommands(temp_player_states);

    // // Trying to blast a bot in an invalid position
    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::INVALID_BLAST_POSITION, _));
    temp_player_states[0].bots[0].final_destination = DoubleVec2D(-10, -5);
    RunCommands(temp_player_states);

    // Trying to transform a bot in an invalid position
    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::INVALID_TRANSFORM_POSITION, _));
    temp_player_states[0].bots[0].transform_destination = DoubleVec2D(-10, -5);
    RunCommands(temp_player_states);

    // ------------- TOWER TESTS ------------------

    // Changing the tower's properties and checking for alter tower property
    // error
    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_TOWER_PROPERTY, _));
    temp_player_states[0].towers[0].hp = -1;
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER1,
                                  ErrorType::NO_ALTER_TOWER_PROPERTY, _));
    temp_player_states[0].towers[0].id = -1;
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER2,
                                  ErrorType::NO_ALTER_TOWER_PROPERTY, _));
    temp_player_states[1].towers[0].position = DoubleVec2D(0, 0);
    RunCommands(temp_player_states);

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger, LogError(PlayerId::PLAYER2,
                                  ErrorType::NO_ALTER_TOWER_PROPERTY, _));
    temp_player_states[1].towers[0].state = player_state::TowerState::BLAST;
    RunCommands(temp_player_states);

    // Maximum tower limit reached
    for (int tower_count = 0;
         tower_count < Constants::Actor::MAX_NUM_TOWERS - 1; ++tower_count) {
        size_t actor_id = Actor::getNextActorId();
        auto state_tower =
            new state::Tower(actor_id, PlayerId::PLAYER1, ActorType::TOWER, 100,
                             100, DoubleVec2D(0, 0), 2, 2, BlastCallback{});
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

    ManageActorExpectations(state_bots, state_towers);
    EXPECT_CALL(*logger,
                LogError(PlayerId::PLAYER1, ErrorType::TOWER_LIMIT_REACHED, _));
    temp_player_states[0].bots[0].transforming = true;
    RunCommands(temp_player_states);
}