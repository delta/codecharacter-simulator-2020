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

// Maximum HP of Bot actor
const uint64_t MAX_BOT_HP = 200;

// Maximum number of bots per player
const size_t MAX_NUM_BOTS = 100;

// Number of bots that each player starts with
const size_t NUM_BOTS_START = 1;

// Blast impact radius around the bot
const uint64_t BOT_BLAST_IMPACT_RADIUS = 2;

// Blast damage of a bot
const size_t BOT_BLAST_DAMAGE_POINTS = 50;

// Speed of a bot in manhattan distance per turn
const uint64_t BOT_SPEED = 1;

// Number of bots spawning at home base per turn
const uint64_t BOT_SPAWN_FREQUENCY = 1;

// Maximum HP of Tower actor
const uint64_t MAX_TOWER_HP = 600;

// Maximum number of towers per player
const size_t MAX_NUM_TOWERS = 150;

// Blast impact radius around the tower
const uint64_t TOWER_BLAST_IMPACT_RADIUS = 6;

// Blast damage of a bot
const size_t TOWER_BLAST_DAMAGE_POINTS = 500;

// Minimum number of turns tower must be alive to be able to blast
const uint64_t TOWER_MIN_BLAST_AGE = 3;

} // namespace Constants::Actor

#pragma GCC diagnostic pop
