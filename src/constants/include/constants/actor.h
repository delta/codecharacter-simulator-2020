/**
 * @file actor.h
 * Constants related to game actors
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++17-extensions"

namespace Constants::Actor {

// Maximum number of bots per player
const size_t MAX_NUM_BOTS = 500;

// Number of bots that each player starts with
const size_t NUM_BOTS_START = 20;

// Blast impact radius around the bot
const size_t BOT_BLAST_IMPACT_RADIUS = 3;

// Speed of a bot in manhattan distance per turn
const int64_t BOT_SPEED = 2;

// Frequency at which bots spawn at home base per turn
const int64_t BOT_SPAWN_FREQUENCY = 1;

// Maximum number of towers per player
const size_t MAX_NUM_TOWERS = 200;

// Blast impact radius around the tower
const size_t TOWER_BLAST_IMPACT_RADIUS = 3;

// Minimum number of turns tower must be alive to be able to blast
const size_t TOWER_MIN_BLAST_AGE = 5;

} // namespace Constants::Actor

#pragma GCC diagnostic pop
