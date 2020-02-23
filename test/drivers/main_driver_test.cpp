#include "constants/constants.h"
#include "drivers/main_driver.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "drivers/timer.h"
#include "logger/mocks/logger_mock.h"
#include "player_wrapper/transfer_state.h"
#include "state/mocks/state_syncer_mock.h"
#include "gtest/gtest.h"
#include <atomic>
#include <cmath>
#include <iostream>
#include <memory>
#include <thread>

using namespace ::testing;
using namespace std;
using namespace state;
using namespace drivers;

class MainDriverTest : public testing::Test {
  protected:
    unique_ptr<MainDriver> driver;

    const static vector<string> shared_memory_names;

    const static int num_turns;

    const static int time_limit_ms;

    const static int turn_instruction_limit;

    const static int game_instruction_limit;

    // Returns a new mock main driver
    static unique_ptr<MainDriver>
    CreateMockMainDriver(unique_ptr<StateSyncerMock> state_syncer_mock,
                         unique_ptr<LoggerMock> v_logger) {
        vector<unique_ptr<SharedMemoryMain>> shms;

        for (const auto &shm_name : shared_memory_names) {
            // Remove shm if it already exists
            boost::interprocess::shared_memory_object::remove(shm_name.c_str());
            // Create new shm
            shms.emplace_back(new SharedMemoryMain(shm_name, false, 0,
                                                   transfer_state::State()));
        }

        return std::make_unique<MainDriver>(
            move(state_syncer_mock), move(shms), turn_instruction_limit,
            game_instruction_limit, num_turns, Timer::Interval(time_limit_ms),
            move(v_logger), "game.log");
    }

  public:
    MainDriverTest() : driver(nullptr) {}
};

const vector<string> MainDriverTest::shared_memory_names = {"ShmTest1",
                                                            "ShmTest2"};
const int MainDriverTest::num_turns = pow(10, 4);
const int MainDriverTest::time_limit_ms = 2000;
const int MainDriverTest::turn_instruction_limit = 5;
const int MainDriverTest::game_instruction_limit = 10;

TEST_F(MainDriverTest, CleanRunByScore) {
    // Expect a lock-free atomic_bool as only they will definitely work in shm.
    EXPECT_EQ(ATOMIC_BOOL_LOCK_FREE, 2);

    // Declaring mock state syncer and setting expectations
    unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

    EXPECT_CALL(*state_syncer_mock, updateMainState(_, _)).Times(num_turns);
    EXPECT_CALL(*state_syncer_mock, updatePlayerStates(_)).Times(num_turns + 1);

    // Expect scores calls
    EXPECT_CALL(*state_syncer_mock, getScores(_))
        .WillOnce(Return(array<int64_t, 2>{10, 10}));

    // Declare mock logger and setting expectations
    unique_ptr<LoggerMock> v_logger(new LoggerMock());

    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
        .Times(num_turns);
    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
        .Times(num_turns);
    EXPECT_CALL(*v_logger, LogFinalGameParams(_, _)).Times(1);
    EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

    driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

    // Start main driver on new thread
    GameResult game_result{};
    thread main_runner([this, &game_result] { game_result = driver->start(); });

    vector<thread> player_runners;
    for (int i = 0; i < 2; ++i) {
        ostringstream command_stream;
        command_stream << "./main_driver_test_player " << shared_memory_names[i]
                       << ' ' << time_limit_ms << ' ' << num_turns << ' '
                       << turn_instruction_limit;
        string command = command_stream.str();
        player_runners.emplace_back(
            [command] { EXPECT_EQ(system(command.c_str()), 0); });
    }

    // Wait for player processes to wrap up
    for (auto &runner : player_runners) {
        runner.join();
    }
    // Wait for main driver to wrap up
    main_runner.join();

    // Everyone gets a score of 10 and status is normal as game finished
    // normally
    EXPECT_EQ(game_result.winner, GameResult::Winner::TIE);
    EXPECT_EQ(game_result.win_type, GameResult::WinType::SCORE);
    for (auto const &result : game_result.player_results) {
        EXPECT_EQ(result.score, 10);
        EXPECT_EQ(result.status, PlayerResult::Status::NORMAL);
    }
}

// Tests for case when player exits/crashes before game ends
// Main driver should simply time out and exit cleanly
TEST_F(MainDriverTest, EarlyPlayerExit) {
    unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

    // Expect only half the number of turns to be run
    EXPECT_CALL(*state_syncer_mock, updateMainState(_, _)).Times(num_turns / 2);
    EXPECT_CALL(*state_syncer_mock, updatePlayerStates(_))
        .Times(num_turns / 2 + 1);

    // Get Scores WILL NOT be called
    EXPECT_CALL(*state_syncer_mock, getScores(_)).Times(0);

    // Logger called num_turns/2 + 1 times before the driver exits
    unique_ptr<LoggerMock> v_logger(new LoggerMock());
    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
        .Times(num_turns / 2);
    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
        .Times(num_turns / 2);

    driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

    GameResult game_result{};
    thread main_runner([this, &game_result] { game_result = driver->start(); });

    vector<thread> player_runners;
    for (int i = 0; i < 2; ++i) {
        ostringstream command_stream;
        command_stream << "./main_driver_test_player " << shared_memory_names[i]
                       << ' ' << time_limit_ms << ' ' << num_turns / 2 << ' '
                       << turn_instruction_limit;
        string command = command_stream.str();
        player_runners.emplace_back(
            [command] { EXPECT_EQ(system(command.c_str()), 0); });
    }

    for (auto &runner : player_runners) {
        runner.join();
    }

    main_runner.join();

    EXPECT_EQ(game_result.winner, GameResult::Winner::NONE);
    EXPECT_EQ(game_result.win_type, GameResult::WinType::NONE);

    // Results are undefined as game didn't complete
    for (auto result : game_result.player_results) {
        EXPECT_EQ(result.status, PlayerResult::Status::UNDEFINED);
    }
}

// Test for when a player exceeds game instruction limit
// Main driver should mark the player as having exceeded instruction limit
TEST_F(MainDriverTest, InstructionLimitReached) {
    unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

    // Expect only half the turns to run
    EXPECT_CALL(*state_syncer_mock, updateMainState(_, _)).Times(num_turns / 2);
    EXPECT_CALL(*state_syncer_mock, updatePlayerStates(_))
        .Times(num_turns / 2 + 1);

    // Get Scores WILL NOT be called
    EXPECT_CALL(*state_syncer_mock, getScores(_)).Times(0);

    unique_ptr<LoggerMock> v_logger(new LoggerMock());
    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
        .Times(num_turns / 2 + 1);
    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
        .Times(num_turns / 2 + 1);
    EXPECT_CALL(*v_logger, LogFinalGameParams(_, _)).Times(1);
    EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

    driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

    GameResult game_result{};
    thread main_runner([this, &game_result] { game_result = driver->start(); });

    vector<thread> player_runners;
    for (int i = 0; i < 2; ++i) {
        ostringstream command_stream;
        command_stream << "./main_driver_test_player " << shared_memory_names[i]
                       << ' ' << time_limit_ms << ' ' << num_turns / 2 - 1
                       << ' ' << turn_instruction_limit;
        string command = command_stream.str();
        player_runners.emplace_back(
            [command] { EXPECT_EQ(system(command.c_str()), 0); });
    }

    for (auto &runner : player_runners) {
        runner.join();
    }

    // Simulating just being under instruction limit on n/2 turn by all
    // players
    for (const auto &shm_name : shared_memory_names) {
        SharedMemoryPlayer shm_player(shm_name);
        SharedBuffer *buf = shm_player.getBuffer();

        while (!buf->is_player_running)
            ;

        buf->instruction_counter = game_instruction_limit;
        buf->is_player_running = false;
    }

    // Simulating instruction limit exceeding on n/2 + 1 turn by all players
    for (const auto &shm_name : shared_memory_names) {
        SharedMemoryPlayer shm_player(shm_name);
        SharedBuffer *buf = shm_player.getBuffer();

        while (!buf->is_player_running)
            ;

        buf->instruction_counter = game_instruction_limit + 1;
        buf->is_player_running = false;
    }

    main_runner.join();

    EXPECT_EQ(game_result.winner, GameResult::Winner::TIE);
    EXPECT_EQ(game_result.win_type,
              GameResult::WinType::EXCEEDED_INSTRUCTION_LIMIT);
    for (auto result : game_result.player_results) {
        EXPECT_EQ(result.status,
                  PlayerResult::Status::EXCEEDED_INSTRUCTION_LIMIT);
    }
}

// Test for when a single player exceeds game instruction limit
// Main driver should mark the player as having exceeded instruction limit and
// the other player should be winner
TEST_F(MainDriverTest, InstructionLimitReachedSinglePlayer) {
    unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

    // Expect only half the turns to run
    EXPECT_CALL(*state_syncer_mock, updateMainState(_, _)).Times(num_turns / 2);
    EXPECT_CALL(*state_syncer_mock, updatePlayerStates(_))
        .Times(num_turns / 2 + 1);

    // Get Scores WILL NOT be called
    EXPECT_CALL(*state_syncer_mock, getScores(_)).Times(0);

    unique_ptr<LoggerMock> v_logger(new LoggerMock());
    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _))
        .Times(num_turns / 2 + 1);
    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _))
        .Times(num_turns / 2 + 1);
    EXPECT_CALL(*v_logger, LogFinalGameParams(_, _)).Times(1);
    EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

    driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

    GameResult game_result{};
    thread main_runner([this, &game_result] { game_result = driver->start(); });

    vector<thread> player_runners;
    for (int i = 0; i < 2; ++i) {
        ostringstream command_stream;
        command_stream << "./main_driver_test_player " << shared_memory_names[i]
                       << ' ' << time_limit_ms << ' ' << num_turns / 2 - 1
                       << ' ' << turn_instruction_limit;
        string command = command_stream.str();
        player_runners.emplace_back(
            [command] { EXPECT_EQ(system(command.c_str()), 0); });
    }

    for (auto &runner : player_runners) {
        runner.join();
    }

    // Simulating just being under instruction limit on n/2 turn by all
    // players
    for (const auto &shm_name : shared_memory_names) {
        SharedMemoryPlayer shm_player(shm_name);
        SharedBuffer *buf = shm_player.getBuffer();

        while (!buf->is_player_running)
            ;

        buf->instruction_counter = game_instruction_limit;
        buf->is_player_running = false;
    }

    SharedMemoryPlayer shm_player_1(shared_memory_names[0]);
    SharedBuffer *buf_player_1 = shm_player_1.getBuffer();
    SharedMemoryPlayer shm_player_2(shared_memory_names[1]);
    SharedBuffer *buf_player_2 = shm_player_2.getBuffer();

    while (!buf_player_1->is_player_running)
        ;

    // Exceed game limit for player1
    buf_player_1->instruction_counter = game_instruction_limit + 1;
    buf_player_1->is_player_running = false;

    while (!buf_player_2->is_player_running)
        ;

    // Just below game limit for player2
    buf_player_2->instruction_counter = game_instruction_limit;
    buf_player_2->is_player_running = false;

    main_runner.join();

    EXPECT_EQ(game_result.winner, GameResult::Winner::PLAYER2);
    EXPECT_EQ(game_result.win_type,
              GameResult::WinType::EXCEEDED_INSTRUCTION_LIMIT);
    EXPECT_EQ(game_result.player_results[0].status,
              PlayerResult::Status::EXCEEDED_INSTRUCTION_LIMIT);
    // Undefined since game hasn't finished
    EXPECT_EQ(game_result.player_results[1].status,
              PlayerResult::Status::UNDEFINED);
}

// Test for cancelling main driver
// Simulate a turn and then check if cancelling works
TEST_F(MainDriverTest, Cancellation) {
    unique_ptr<StateSyncerMock> state_syncer_mock(new StateSyncerMock());

    // Expect only one turn to run
    EXPECT_CALL(*state_syncer_mock, updateMainState(_, _)).Times(1);
    EXPECT_CALL(*state_syncer_mock, updatePlayerStates(_)).Times(2);

    // Get Scores WILL NOT be called
    EXPECT_CALL(*state_syncer_mock, getScores(_)).Times(0);

    unique_ptr<LoggerMock> v_logger(new LoggerMock());
    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER1, _)).Times(1);
    EXPECT_CALL(*v_logger, LogInstructionCount(PlayerId::PLAYER2, _)).Times(1);
    EXPECT_CALL(*v_logger, LogFinalGameParams(_, _)).Times(1);
    EXPECT_CALL(*v_logger, WriteGame(_)).Times(1);

    driver = CreateMockMainDriver(move(state_syncer_mock), move(v_logger));

    GameResult game_result{};
    thread main_runner([this, &game_result] { game_result = driver->start(); });

    // Simulating one turn
    for (const auto &shm_name : shared_memory_names) {
        SharedMemoryPlayer shm_player(shm_name);
        SharedBuffer *buf = shm_player.getBuffer();
        while (!buf->is_player_running)
            ;
        buf->is_player_running = false;
    }

    SharedMemoryPlayer shm_player(shared_memory_names[0]);
    SharedBuffer *buf = shm_player.getBuffer();
    while (!buf->is_player_running)
        ;

    driver->cancel();

    main_runner.join();

    EXPECT_EQ(game_result.winner, GameResult::Winner::NONE);
    EXPECT_EQ(game_result.win_type, GameResult::WinType::NONE);
    for (auto result : game_result.player_results) {
        EXPECT_EQ(result.status, PlayerResult::Status::UNDEFINED);
    }
}
