/**
 * @file simulator.h
 * Add constants related to Simulation
 */

#pragma once

#include <array>

namespace Constants {
namespace Simulator {

// Instruction limit per turn exceeding which turn is cancelled
const uint64_t PLAYER_INSTRUCTION_LIMIT_TURN = 10000000;

// Number of instructions exceeding which the entire game is cancelled
const uint64_t PLAYER_INSTRUCTION_LIMIT_GAME =
    100 * PLAYER_INSTRUCTION_LIMIT_TURN;

// Number of turns in the game
const int64_t NUM_TURNS = 1000;

// Duration of the game in milliseconds
const int64_t GAME_DURATION_MS = 20 * 1000;

// File names for passing SHM names to player processes
const auto SHM_FILE_NAMES = std::array<std::string, 2>{"shm1.txt", "shm2.txt"};

// File where the output game binary log will be stored
const auto GAME_LOG_FILE_NAME = "game.log";

// Shared buffer size in bytes
const size_t SHARED_BUFFER_SIZE = 262143;

} // namespace Simulator
} // namespace Constants
