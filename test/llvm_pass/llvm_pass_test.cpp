#include "drivers/player_driver.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "player_code/test/player_code_test_0.h"
#include "player_code/test/player_code_test_1.h"
#include "player_code/test/player_code_test_2.h"
#include "player_wrapper/player_code_wrapper.h"
#include "gtest/gtest.h"
#include <fstream>

using namespace drivers;
using namespace player_wrapper;
using namespace player_code;
using namespace std;
using namespace ::testing;

class LLVMPassTest : public Test {
  protected:
    const string shm_name = "LLVMInstTest";

    const string log_file = "log_file.log";

    const string turn_prefix = "<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>\n";

    const string truncate_message = "(truncated since word limit exceeded)\n";

    unique_ptr<SharedMemoryMain> shm_main;

    SharedBuffer *buf;

    unique_ptr<PlayerDriver> player_driver;

  public:
    LLVMPassTest() {
        // Remove if SHM with name already exists
        boost::interprocess::shared_memory_object::remove(shm_name.c_str());

        this->shm_main = make_unique<SharedMemoryMain>(shm_name, false, 0,
                                                       transfer_state::State());
        this->buf = shm_main->getBuffer();
    }

    template <class T>
    void setPlayerDriver(int num_turns, int time_limit_ms,
                         int64_t max_log_turn_length) {
        unique_ptr<SharedMemoryPlayer> shm_player(
            new SharedMemoryPlayer(shm_name));

        unique_ptr<T> player_code(new T());

        unique_ptr<PlayerCodeWrapper> player_code_wrapper(
            new PlayerCodeWrapper(move(player_code)));

        this->player_driver = make_unique<PlayerDriver>(
            move(player_code_wrapper), move(shm_player), num_turns,
            Timer::Interval(time_limit_ms), log_file, turn_prefix,
            truncate_message, max_log_turn_length);
    }
};

// The below tests are for both the PlayerDriver and the instrumentation

// Test case to see if instrumentation works on a normal run
TEST_F(LLVMPassTest, NormalTest1) {
    setPlayerDriver<PlayerCode0>(2, 1000, 0);

    // Start the player driver
    thread runner([this] { player_driver->start(); });

    buf->instruction_counter = 0;

    buf->is_player_running = true;
    while (buf->is_player_running)
        ;

    int inst_count_one = player_driver->getCount();

    buf->instruction_counter = 0;

    buf->is_player_running = true;
    while (buf->is_player_running)
        ;

    int inst_count_two = player_driver->getCount();

    runner.join();

    ASSERT_GT(buf->instruction_counter, 0);
    ASSERT_EQ(buf->instruction_counter, inst_count_two);
    ASSERT_EQ(inst_count_two, inst_count_one);
}

// Test case for game timing out before all turns complete
// Player driver needs to exit gracefully
// Instrumentation needs to work correctly
TEST_F(LLVMPassTest, PlayerDriverTimeout) {
    int num_turns = 50;
    int time_limit_ms = 1500;

    setPlayerDriver<PlayerCode0>(num_turns, time_limit_ms, 0);

    buf->instruction_counter = 0;

    thread runner([this] { player_driver->start(); });

    Timer timer;
    atomic_bool is_timed_out(false);
    timer.start((Timer::Interval(time_limit_ms)),
                [&is_timed_out]() { is_timed_out = true; });

    buf->is_player_running = true;
    while (buf->is_player_running && !is_timed_out)
        ;
    int prev_instruction_count = buf->instruction_counter;

    for (int i = 1; i < num_turns; ++i) {
        buf->is_player_running = true;
        while (buf->is_player_running && !is_timed_out)
            ;
        // Number of instructions every turn must be the same, as the exact same
        // player code runs every turn
        EXPECT_EQ(prev_instruction_count, buf->instruction_counter);
        prev_instruction_count = buf->instruction_counter;
    }

    EXPECT_EQ(is_timed_out, true);

    runner.join();
    timer.stop();
}

// Normal test for another player code
TEST_F(LLVMPassTest, NormalTest2) {
    int num_turns = 50;
    int time_limit_ms = 1000;

    setPlayerDriver<PlayerCode1>(num_turns, time_limit_ms, 0);

    buf->instruction_counter = 0;

    thread runner([this] { player_driver->start(); });

    Timer timer;
    atomic_bool is_timed_out(false);
    timer.start((Timer::Interval(time_limit_ms)),
                [&is_timed_out]() { is_timed_out = true; });

    buf->is_player_running = true;
    while (buf->is_player_running && !is_timed_out)
        ;
    int prev_instruction_count = buf->instruction_counter;
    EXPECT_GT(prev_instruction_count, 0);

    for (int i = 1; i < num_turns; ++i) {
        buf->is_player_running = true;
        while (buf->is_player_running && !is_timed_out)
            ;
        // Number of instructions every turn must be the same, as the exact same
        // player code runs every turn
        EXPECT_EQ(prev_instruction_count, buf->instruction_counter);
        prev_instruction_count = buf->instruction_counter;
        buf->instruction_counter = 0;
    }

    runner.join();
    timer.stop();
}

// Test to see if debug log file is properly written to
TEST_F(LLVMPassTest, DebugLogsNormalRun) {
    int num_turns = 50;
    int time_limit_ms = 1000;
    int max_log_turn_length = 10E5;

    // Run the player driver
    setPlayerDriver<PlayerCode2>(num_turns, time_limit_ms, max_log_turn_length);

    thread runner([this] { player_driver->start(); });

    for (int i = 0; i < num_turns; ++i) {
        buf->is_player_running = true;
        while (buf->is_player_running)
            ;
    }

    runner.join();

    // Player is done running

    // Open the log file and expect it to exist
    ifstream log_file(this->log_file);
    EXPECT_TRUE(log_file.good());

    // Read the log file
    std::ostringstream logs;
    logs << log_file.rdbuf();
    auto log_str = logs.str();

    // Check if it has the expected contents: turn prefix and 10^4 asterisks
    // every turn
    size_t pos = 0;
    int num_turns_in_log = 0;
    while ((pos = log_str.find(this->turn_prefix)) != std::string::npos) {
        log_str.erase(0, pos + this->turn_prefix.length());
        EXPECT_EQ(log_str.substr(0, 10E4), std::string(10E4, '*'));
        num_turns_in_log++;
    }

    // Expect all turns to be logged
    EXPECT_EQ(num_turns_in_log, num_turns);

    // Delete log file
    log_file.close();
    EXPECT_EQ(std::remove(this->log_file.c_str()), 0);
}

// Test to see if debug logs are properly handling per turn character limits
TEST_F(LLVMPassTest, DebugLogsTruncated) {
    int num_turns = 50;
    int time_limit_ms = 1000;
    int max_log_turn_length = 10E3;

    // Run the player driver
    setPlayerDriver<PlayerCode2>(num_turns, time_limit_ms, max_log_turn_length);

    thread runner([this] { player_driver->start(); });

    for (int i = 0; i < num_turns; ++i) {
        buf->is_player_running = true;
        while (buf->is_player_running)
            ;
    }

    runner.join();

    // Player is done running

    // Open the log file and expect it to exist
    ifstream log_file(this->log_file);
    EXPECT_TRUE(log_file.good());

    // Read the log file
    std::ostringstream logs;
    logs << log_file.rdbuf();
    auto log_str = logs.str();

    // Check if it has the expected contents: turn prefix and the truncation
    // message
    size_t pos = 0;
    int num_turns_in_log = 0;
    while ((pos = log_str.find(this->turn_prefix)) != std::string::npos) {
        log_str.erase(0, pos + this->turn_prefix.length());
        EXPECT_EQ(log_str.find(this->truncate_message), max_log_turn_length);
        num_turns_in_log++;
    }

    // Expect all turns to be logged
    EXPECT_EQ(num_turns_in_log, num_turns);

    // Delete log file
    log_file.close();
    EXPECT_EQ(std::remove(this->log_file.c_str()), 0);
}
