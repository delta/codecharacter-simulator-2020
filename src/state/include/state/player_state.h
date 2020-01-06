/**
 * @file player_state.h
 * Player structure for game operations
 */

#pragma once

#include "constants/constants.h"
#include "physics/vector.hpp"
#include "state/interfaces/i_updatable.h"
#include "state/utilities.h"

// #include <iostream>
#include <array>

namespace player_state {
using TerrainType = state::TerrainType;

enum class BotState : int8_t {
  // Bot is idle, i.e doing  nothing
  IDLE,
  // Bot is moving
  MOVE,
  // Bot is dead
  DEAD
};

std::ostream &operator<<(std::ostream &os, BotState bot_state) {
  switch (bot_state) {
  case BotState::IDLE:
    os << "IDLE";
    break;
  case BotState::MOVE:
    os << "MOVE";
    break;
  case BotState::DEAD:
    os << "DEAD";
    break;
  }

  return os;
}

enum class TowerState : int8_t {
  // Tower is idle, i.e doing nothing
  IDLE,
  // Tower is dead
  DEAD
};

std::ostream &operator<<(std::ostream &os, TowerState tower_state) {
  switch (tower_state) {
  case TowerState::IDLE:
    os << "IDLE";
    break;
  case TowerState::DEAD:
    os << "DEAD";
    break;
  }

  return os;
}

struct _Actor {
  int64_t id;
	physics::Vector<int64_t> position;
	int64_t hp;
};

struct Bot : _Actor{

};

std::ostream &operator<<(std::ostream &os, Bot bot) {
  // complete this, based on Bot definition
}

struct Tower : _Actor {};

std::ostream &operator<<(std::ostream &os, Tower tower) {
  // complete this, based on Tower defintion.
}


struct MapElement {
  TerrainType type;
};

/**
 * Main Player state, the struct interface available to each player.
 */
struct State {
  std::array<std::array<MapElement, Constants::Map::MAP_SIZE>, Constants::Map::MAP_SIZE> map;

  std::array<Bot, Constants::Actor::MAX_NUM_BOTS> bots;
  std::array<Bot, Constants::Actor::MAX_NUM_BOTS> enemy_bots;

  int64_t num_bots;
  int64_t num_enemy_bots;

  std::array<Tower, Constants::Map::MAP_SIZE*Constants::Map::MAP_SIZE> towers;
  std::array<Tower, Constants::Map::MAP_SIZE*Constants::Map::MAP_SIZE> enemy_towers;

  int64_t num_towers;
  int64_t num_enemy_towers;

  int64_t score;
  int64_t interestingness;
  int64_t interest_threshold;
};



std::ostream &operator<<(std::ostream &os, State state) {
	using std::endl;

	os << "Map:" << endl;
	for (auto const &row : state.map) {
		for (auto const &elem : row) {
			switch (elem.type) {
			case TerrainType::LAND:
				os << "L"
				   << " ";
				break;
			case TerrainType::WATER:
				os << "W"
				   << " ";
				break;
			case TerrainType::FLAG:
				os << "F"
				   << " ";
				break;
			}
		}
		os << endl;
	}

	os << "-- Bots --" << endl;
	for (auto const &bot : state.bots) {
		os << bot << endl;
	}

  os << "-- Enemy Bots --" << endl;
	for (auto const &enemy_bot : state.enemy_bots) {
		os << enemy_bot << endl;
	}


	os << "-- Towers --" << endl;
	for (auto const &tower : state.towers) {
		os << tower << endl;
	}


	os << "-- Enemy Tower --" << endl;
	for (auto const &enemy_tower : state.enemy_towers) {
		os << enemy_tower << endl;
	}

	os << "-- Score --" << endl;
	os << state.score << endl;

	return os;
}

} // namespace player_state
