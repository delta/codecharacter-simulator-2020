#include "constants/constants.h"
#include "logger/logger.h"
#include "state/actor/bot_states/bot_move_state.h"
#include "state/mocks/state_mock.h"
#include "state/score_manager/score_manager.h"
#include "state/utilities.h"
#include "gtest/gtest.h"
#include <fstream>

using namespace std;
using namespace testing;
using namespace state;
using namespace logger;
using namespace Constants::Simulator;
using namespace Constants::Actor;

const auto TEST_MAP_SIZE = size_t{5};

const auto L = TerrainType::LAND;
const auto W = TerrainType::WATER;
const auto F = TerrainType::FLAG;

class LoggerTest : public testing::Test {
  protected:
    std::unique_ptr<StateMock> state;
    std::unique_ptr<Logger> logger;

    std::unique_ptr<ScoreManager> score_manager;
    std::unique_ptr<PathPlanner> path_planner;

    std::unique_ptr<Map> map;

    LoggerTest()
        : state(make_unique<StateMock>()),
          logger(make_unique<Logger>(state.get(), PLAYER_INSTRUCTION_LIMIT_TURN,
                                     PLAYER_INSTRUCTION_LIMIT_GAME, MAX_BOT_HP,
                                     MAX_TOWER_HP)) {
        auto map_matrix = vector<vector<TerrainType>>{{
            {L, L, F, L, L},
            {L, L, F, L, L},
            {L, L, F, L, L},
            {W, L, L, L, W},
            {W, W, W, W, W},
        }};
        map = make_unique<Map>(map_matrix, TEST_MAP_SIZE);

        this->score_manager = make_unique<ScoreManager>();
    }
};

TEST_F(LoggerTest, WriteReadTest) {

    // Create and assign bots for each player
    auto *bot_1 = new Bot(state::PlayerId::PLAYER1, MAX_BOT_HP, MAX_TOWER_HP,
                          DoubleVec2D(1, 1), BOT_SPEED, BOT_BLAST_IMPACT_RADIUS,
                          0, score_manager.get(), path_planner.get(),
                          BlastCallback(), ConstructTowerCallback());
    auto *bot_2 = new Bot(state::PlayerId::PLAYER1, MAX_BOT_HP, MAX_TOWER_HP,
                          DoubleVec2D(2, 1), BOT_SPEED, BOT_BLAST_IMPACT_RADIUS,
                          0, score_manager.get(), path_planner.get(),
                          BlastCallback(), ConstructTowerCallback());
    auto *bot_3 = new Bot(state::PlayerId::PLAYER2, MAX_BOT_HP, MAX_TOWER_HP,
                          DoubleVec2D(2, 2), BOT_SPEED, BOT_BLAST_IMPACT_RADIUS,
                          0, score_manager.get(), path_planner.get(),
                          BlastCallback(), ConstructTowerCallback());

    std::array<vector<Bot *>, 2> bots;
    vector<Bot *> player_1_bots;
    vector<Bot *> player_2_bots;

    player_1_bots.push_back(bot_1);
    player_1_bots.push_back(bot_2);
    player_2_bots.push_back(bot_3);
    bots[0] = player_1_bots;
    bots[1] = player_2_bots;

    // Creating one tower for each player
    auto *tower_1 = new Tower(PlayerId::PLAYER1, MAX_TOWER_HP, MAX_TOWER_HP,
                              DoubleVec2D(1, 2), 0, TOWER_BLAST_IMPACT_RADIUS,
                              score_manager.get(), BlastCallback());
    auto *tower_2 = new Tower(PlayerId::PLAYER2, MAX_TOWER_HP, MAX_TOWER_HP,
                              DoubleVec2D(2, 3), 0, TOWER_BLAST_IMPACT_RADIUS,
                              score_manager.get(), BlastCallback());

    // Create and assign one tower for each player
    std::array<vector<Tower *>, 2> towers;
    vector<Tower *> player_1_towers;
    vector<Tower *> player_2_towers;

    player_1_towers.push_back(tower_1);
    player_2_towers.push_back(tower_2);

    towers[0] = player_1_towers;
    towers[1] = player_2_towers;

    /// Simulate movement of bot

    // Bot 1 of player 1 moves to (1, 2) and switches to MOVE state
    std::array<vector<Bot *>, 2> bots_2 = bots;
    bots_2[0][0] = new Bot(
        bot_1->getActorId(), state::PlayerId::PLAYER1, MAX_BOT_HP, MAX_TOWER_HP,
        BotStateName::MOVE, DoubleVec2D(1, 2), BOT_SPEED,
        BOT_BLAST_IMPACT_RADIUS, 0, score_manager.get(), path_planner.get(),
        BlastCallback(), ConstructTowerCallback(), false, false);

    // Bot 1 of player 1 moves to (1, 3) and switches back to IDLE state
    std::array<vector<Bot *>, 2> bots_3 = bots_2;
    bots_3[0][0] = new Bot(
        bot_1->getActorId(), state::PlayerId::PLAYER1, MAX_BOT_HP, MAX_TOWER_HP,
        BotStateName::IDLE, DoubleVec2D(1, 3), BOT_SPEED,
        BOT_BLAST_IMPACT_RADIUS, 0, score_manager.get(), path_planner.get(),
        BlastCallback(), ConstructTowerCallback(), false, false);

    /// Simulate blast of bot

    // Bot 2 of player 1 has blasted and moved to dead state
    std::array<vector<Bot *>, 2> bots_4 = bots_3;
    bots_4[0][1] = new Bot(
        bot_2->getActorId(), state::PlayerId::PLAYER1, MAX_BOT_HP, MAX_TOWER_HP,
        BotStateName::DEAD, DoubleVec2D(2, 1), BOT_SPEED,
        BOT_BLAST_IMPACT_RADIUS, 0, score_manager.get(), path_planner.get(),
        BlastCallback(), ConstructTowerCallback(), true, false);

    // Blast tower 1 of player 1 and move to dead state
    std::array<vector<Tower *>, 2> towers_2 = towers;
    towers_2[0][0] = new Tower(tower_1->getActorId(), PlayerId::PLAYER2,
                               TowerStateName::DEAD, MAX_TOWER_HP, MAX_TOWER_HP,
                               DoubleVec2D(2, 3), 0, TOWER_BLAST_IMPACT_RADIUS,
                               BlastCallback(), score_manager.get(), true);

    EXPECT_CALL(*state, getMap()).WillOnce(Return(map.get()));
    EXPECT_CALL(*state, getTowers())
        .WillOnce(Return(towers))
        .WillRepeatedly(Return(towers_2));

    EXPECT_CALL(*state, getBots())
        .WillOnce(Return(bots))
        .WillOnce(Return(bots_2))
        .WillOnce(Return(bots_3))
        .WillRepeatedly(Return(bots_4));

    std::array<uint64_t, 2> scores1 = {100, 100};
    std::array<uint64_t, 2> scores2 = {300, 200};
    EXPECT_CALL(*state, getScoreManager()).Times(4).WillRepeatedly(Return(score_manager.get()));
    EXPECT_CALL(*state, getScores())
        .WillOnce(Return(scores1))
        .WillRepeatedly(Return(scores2));


    vector<int64_t> inst_counts = {123456, 654321};
    logger->logInstructionCount(PlayerId::PLAYER1, inst_counts[0]);
    logger->logInstructionCount(PlayerId::PLAYER2, inst_counts[1]);

    logger->logError(PlayerId::PLAYER1, ErrorType::INVALID_MOVE_POSITION,
                     "Sample Error 1");
    logger->logError(PlayerId::PLAYER1, ErrorType::INVALID_BLAST_POSITION,
                     "Sample Error 2");
    logger->logError(PlayerId::PLAYER2, ErrorType::INVALID_TRANSFORM_POSITION,
                     "Sample Error 3");
    logger->logError(PlayerId::PLAYER2, ErrorType::NO_ALTER_BOT_PROPERTY,
                     "Sample Error 4");

    logger->logState();
    logger->logState();
    logger->logState();
    logger->logState();

    logger->logFinalGameParams(PlayerId::PLAYER_NULL, {300, 200});

    ostringstream str_stream;
    logger->writeGame(str_stream);
    string result_string = str_stream.str();

    auto game = make_unique<proto::Game>();
    game->ParseFromString(result_string);

    // Check for terrain properties
    ASSERT_EQ(game->map_size(), TEST_MAP_SIZE);
    ASSERT_EQ(game->map_elements(0), proto::LAND);
    ASSERT_EQ(game->map_elements(7), proto::FLAG);
    ASSERT_EQ(game->map_elements(15), proto::WATER);

    // Check if scores have been assigned correctly
    ASSERT_EQ(game->states(0).scores(0), 100);
    ASSERT_EQ(game->states(0).scores(1), 100);
    ASSERT_EQ(game->states(2).scores(0), 300);
    ASSERT_EQ(game->states(2).scores(1), 200);

    // Check if bots are constructed
    ASSERT_EQ(game->states(0).bots_size(), 3);
    ASSERT_EQ(game->states(1).bots_size(), 3);

    // Check if instruction count is constructed
    ASSERT_EQ(game->states(0).instruction_counts_size(), 2);
    ASSERT_EQ(game->states(0).instruction_counts(0), inst_counts[0]);
    ASSERT_EQ(game->states(0).instruction_counts(1), inst_counts[1]);

    // Ensure instruction counts are cleared on next turn
    ASSERT_EQ(game->states(1).instruction_counts_size(), 2);
    ASSERT_EQ(game->states(1).instruction_counts(1), 0);
    ASSERT_EQ(game->states(1).instruction_counts(0), 0);

    // Check if the errors got logged on the first turn
    // Error codes should increment from 0
    // Player 1 errors
    ASSERT_EQ(game->states(0).player_errors(0).errors_size(), 2);
    ASSERT_EQ(game->states(0).player_errors(0).errors(0), 0);
    ASSERT_EQ(game->states(0).player_errors(0).errors(1), 1);
    // Player 2 errors
    ASSERT_EQ(game->states(0).player_errors(1).errors_size(), 2);
    ASSERT_EQ(game->states(0).player_errors(1).errors(0), 2);
    ASSERT_EQ(game->states(0).player_errors(1).errors(1), 3);
    ASSERT_EQ(game->states[0].player)

    // Check if the mapping got set and the message string matches
    auto error_map = *game->mutable_error_map();
    ASSERT_EQ(error_map[game->states(0).player_errors(0).errors(0)],
              "INVALID_MOVE_POSITION: Sample Error 1");
    ASSERT_EQ(error_map[game->states(0).player_errors(0).errors(1)],
              "INVALID_BLAST_POSITION: Sample Error 2");
    ASSERT_EQ(error_map[game->states(0).player_errors(1).errors(0)],
              "INVALID_TRANSFORM_POSITION: Sample Error 3");
    ASSERT_EQ(error_map[game->states(0).player_errors(1).errors(1)],
              "NO_ALTER_BOT_PROPERTY: Sample Error 4");

    // Check if all bots are there in first turn
    ASSERT_EQ(game->states(0).bots(0).id(), bot_1->getActorId());
    ASSERT_EQ(game->states(0).bots(1).id(), bot_2->getActorId());

    // Check if all towers are there in first turn
    ASSERT_EQ(game->states(0).towers(0).id(), tower_1->getActorId());
    ASSERT_EQ(game->states(0).towers(1).id(), tower_2->getActorId());

    // Check if bot 1 moves
    ASSERT_EQ(game->states(0).bots(0).x(), 1);
    ASSERT_EQ(game->states(0).bots(0).y(), 1);
    ASSERT_EQ(game->states(0).bots(0).state(), proto::BOT_IDLE);
    ASSERT_EQ(game->states(1).bots(0).x(), 1);
    ASSERT_EQ(game->states(1).bots(0).y(), 2);
    ASSERT_EQ(game->states(1).bots(0).state(), proto::BOT_MOVE);
    ASSERT_EQ(game->states(2).bots(0).x(), 1);
    ASSERT_EQ(game->states(2).bots(0).y(), 3);
    ASSERT_EQ(game->states(2).bots(0).state(), proto::BOT_IDLE);

    // Check if bot 2 blasts
    ASSERT_EQ(game->states(3).bots(1).state(), proto::BOT_DEAD);
    ASSERT_EQ(game->states(3).bots(1).blast_bot(), true);

    // Check if tower blasts
    ASSERT_EQ(game->states(1).towers(0).state(), proto::TOWER_DEAD);
}
