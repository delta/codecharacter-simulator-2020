/**
 * @file actor.h
 * Constants related to game actors
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace Constants::Actor {

// Maximum number of bots per player
const size_t MAX_NUM_BOTS = 500;

// Number of bots that each player starts with
const size_t NUM_BOTS_START = 20;

// Blast impact radius around the bot (Manhattan distance)
const size_t BLAST_IMPACT_RADIUS = 3;

// Speed of a bot in manhattan distance per turn
const int64_t BOT_SPEED = 2;

// Frequency at which bots spawn at home base per turn
const int64_t BOT_SPAWN_FREQUENCY = 1;

} // namespace Constants::Actor
