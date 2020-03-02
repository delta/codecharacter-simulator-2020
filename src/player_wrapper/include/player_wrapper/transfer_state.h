/**
 * @file transfer_state.h
 * Defines a version of the player state that can be written to SHM
 * TODO: Move transfer_state to drivers
 */

// SHM cannot handle vectors, so this version of the player state uses arrays
// (stack-allocated) instead

#pragma once

#include "state/player_state.h"

#include <algorithm>

namespace transfer_state {

using player_state::BotState;
using player_state::TowerState;

using player_state::Bot;
using player_state::MapElement;
using player_state::Tower;

using namespace Constants;

struct State {
    std::array<std::array<MapElement, Map::MAP_SIZE>, Map::MAP_SIZE> map;
    std::array<DoubleVec2D, Map::MAP_SIZE * Map::MAP_SIZE> flag_offsets;
    size_t num_flags;

    std::array<Bot, Actor::MAX_NUM_BOTS> bots;
    std::array<Bot, Actor::MAX_NUM_BOTS> enemy_bots;
    size_t num_bots;
    size_t num_enemy_bots;

    std::array<Tower, Actor::MAX_NUM_TOWERS> towers;
    std::array<Tower, Actor::MAX_NUM_TOWERS> enemy_towers;
    size_t num_towers;
    size_t num_enemy_towers;

    array<uint64_t, 2> scores;
};

template <typename T, size_t _>
inline std::vector<T> arrayToVector(const std::array<T, _> &arr,
                                    const size_t size) {
    return std::vector<T>(arr.begin(), arr.begin() + size);
}

inline player_state::State
ConvertToPlayerState(const transfer_state::State &ts) {
    auto ps = player_state::State{};

    // Copy Map
    ps.map = ts.map;

    // Copy Bots
    ps.bots.clear();
    ps.bots = arrayToVector(ts.bots, ts.num_bots);

    ps.enemy_bots.clear();
    ps.enemy_bots = arrayToVector(ts.enemy_bots, ts.num_enemy_bots);

    // Copy Towers
    ps.towers.clear();
    ps.towers = arrayToVector(ts.towers, ts.num_towers);

    ps.enemy_towers.clear();
    ps.enemy_towers = arrayToVector(ts.enemy_towers, ts.num_enemy_towers);

    // Copy flag offset positions
    ps.flag_offsets.clear();
    ps.flag_offsets = arrayToVector(ts.flag_offsets, ts.num_flags);

    // Copy score
    std::copy(ts.scores.begin(), ts.scores.end(), ps.scores.begin());

    return ps;
}

template <size_t N, typename T>
std::array<T, N> vectorToArray(const std::vector<T> &vec) {
    auto arr = std::array<T, N>{};
    std::copy_n(vec.begin(), vec.size(), arr.begin());
    return arr;
}

inline transfer_state::State
ConvertToTransferState(const player_state::State &ps) {
    auto ts = transfer_state::State{};

    // Copy map
    ts.map = ps.map;

    // Copy bots
    ts.bots = vectorToArray<Actor::MAX_NUM_BOTS>(ps.bots);
    ts.enemy_bots = vectorToArray<Actor::MAX_NUM_BOTS>(ps.enemy_bots);

    // Copy Towers
    ts.towers = vectorToArray<Actor::MAX_NUM_TOWERS>(ps.towers);
    ts.enemy_towers = vectorToArray<Actor::MAX_NUM_TOWERS>(ps.enemy_towers);

    // Copy flag offsets
    ts.flag_offsets = vectorToArray<Map::MAP_SIZE * Map::MAP_SIZE>(ps.flag_offsets);

    // Copy score
    std::copy(ps.scores.begin(), ps.scores.end(), ts.scores.begin());

    // Copy sizes
    ts.num_bots = ps.bots.size();
    ts.num_enemy_bots = ps.enemy_bots.size();
    ts.num_towers = ps.towers.size();
    ts.num_enemy_towers = ps.enemy_towers.size();
    ts.num_flags = ps.flag_offsets.size();

    return ts;
}

} // namespace transfer_state
