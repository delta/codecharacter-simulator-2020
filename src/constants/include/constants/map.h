/**
 * @file map.h
 * Constants related to map
 */

#pragma once

#include "physics/vector.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++17-extensions"

namespace Constants::Map {

// Size of the map
const size_t MAP_SIZE = 30;

// Position of home bases of players
// Player 2 base position is flipped position of player 1
const auto PLAYER1_BASE_POSITION = DoubleVec2D{0.5, 0.5};
const auto PLAYER2_BASE_POSITION = DoubleVec2D{
    MAP_SIZE - PLAYER1_BASE_POSITION.x, MAP_SIZE - PLAYER1_BASE_POSITION.y};

// Base positions of both players
const std::vector<DoubleVec2D> PLAYER_BASE_POSITIONS = {PLAYER1_BASE_POSITION,
                                                        PLAYER2_BASE_POSITION};

} // namespace Constants::Map

#pragma GCC diagnostic pop
