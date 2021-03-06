/**
 * @file main_driver.h
 * Declaration for main driver class that drives the game
 */

#pragma once

#include "drivers/drivers_export.h"
#include "drivers/game_result.h"
#include "drivers/shared_memory_utils/shared_buffer.h"
#include "drivers/shared_memory_utils/shared_memory_main.h"
#include "drivers/timer.h"
#include "logger/interfaces/i_logger.h"
#include "player_wrapper/transfer_state.h"
#include "state/interfaces/i_state_syncer.h"

#include <atomic>
#include <string>
#include <thread>

namespace drivers {

/**
 * Drives the game by syncing states
 */
class DRIVERS_EXPORT MainDriver {
  private:
    /**
     * Syncs the player's copy of the state with the main one
     */
    std::unique_ptr<state::IStateSyncer> state_syncer;

    /**
     * Shared memories via which player and main drivers communicate
     */
    std::vector<std::unique_ptr<SharedMemoryMain>> shared_memories;

    /**
     * Access pointer to the shared memory
     */
    std::vector<SharedBuffer *> shared_buffers;

    /**
     * Pointers to transfer state copies
     */
    std::array<transfer_state::State *, 2> transfer_states;

    /**
     * Current player states, that are being synced with main state
     */
    std::array<player_state::State, 2> player_states;

    /**
     * Instruction count limit per turn per player
     *
     * If player exceeds limit, the player forfeits the turn.
     */
    uint64_t player_instruction_limit_turn;

    /**
     * Instruction count limit per game per player
     *
     * If player exceeds limit, the player forfeits the game.
     */
    uint64_t player_instruction_limit_game;

    /**
     * Number of turns in the game
     */
    uint64_t num_game_turns;

    /**
     * Process IDs of the two player processes
     */
    std::array<int, 2> process_pids;

    /**
     * true if the game has timed out without completing and timer.start has
     * been called, false otherwise
     */
    std::atomic_bool is_game_timed_out;

    /**
     * Timer for the game
     */
    Timer game_timer;

    /**
     * Time limit for the game.
     * Game must be completed within this time to be considered valid.
     */
    Timer::Interval game_duration;

    /**
     * Instance of logger to write game to log file
     */
    std::unique_ptr<logger::ILogger> logger;

    /**
     * Filename to write the final game log to
     */
    std::string log_file_name;

    /**
     * Flag that is set to cancel the game
     */
    std::atomic_bool cancel_flag;

    /**
     * Return the game scores from the state syncer as a PlayerResults array
     *
     * @return std::array<PlayerResult, 2> PlayerResults
     */
    std::array<PlayerResult, 2> getPlayerResults();

    /**
     * Write final game parameters and stop the timer
     *
     * @param player_id of the winner
     * @param final_scores Scores of players
     */
    void endGame(state::PlayerId player_id = state::PlayerId::PLAYER1,
                 std::array<uint64_t, 2> final_scores = {0, 0});

    /**
     * Blocking function that runs the game
     *
     * @return GameResult object with winner, win type, and player results
     */
    GameResult run();

  public:
    /**
     * Constructor
     */
    MainDriver(std::unique_ptr<state::IStateSyncer> state_syncer,
               std::vector<std::unique_ptr<SharedMemoryMain>> shared_memories,
               int64_t player_instruction_limit_turn,
               int64_t player_instruction_limit_game, int64_t num_game_turns,
               Timer::Interval game_duration,
               std::unique_ptr<logger::ILogger> logger,
               std::string log_file_name);

    /**
     * Set player process ids
     * @param pids Array of PIds of 2 player processes
     */
    void setPids(std::array<int, 2> pids);

    /**
     * Blocking function that starts the game.
     *
     * Calls MainDriver::Run after setting up.
     *
     * @return     GameResult object with winner, win type, and player results
     */
    GameResult start();

    /**
     * Cancels the execution of the main driver.
     *
     * Blocks until main driver fully exits and returns player results
     */
    void cancel();
};
} // namespace drivers
