/**
 * @file player_state.h
 * Player structure for game operations
 */

#pragma once

#include "constants/constants.h"
#include "physics/vector.hpp"
#include "state/utilities.h"

#include <array>

namespace player_state {

enum class TerrainType { LAND, WATER, TOWER, FLAG };

enum class BotState : int8_t {
    // Bot is doing  nothing
    IDLE,
    // Bot is moving
    MOVE,
    // Bot is blowing up.
    BLAST,
    // Bot is changing into Tower
    TRANSFORM,
    // Bot is dead
    DEAD
};

inline std::ostream &operator<<(std::ostream &os, BotState bot_state) {
    switch (bot_state) {
    case BotState::IDLE:
        os << "IDLE";
        break;
    case BotState::MOVE:
        os << "MOVE";
        break;
    case BotState::BLAST:
        os << "BLAST";
        break;
    case BotState::TRANSFORM:
        os << "TRANSFORM";
        break;
    case BotState::DEAD:
        os << "DEAD";
        break;
    }

    return os;
}

enum class TowerState : int8_t {
    // Tower is doing nothing
    IDLE,
    // Tower is blowing up.
    BLAST,
    // Tower is dead
    DEAD
};

inline std::ostream &operator<<(std::ostream &os, TowerState tower_state) {
    switch (tower_state) {
    case TowerState::IDLE:
        os << "IDLE";
        break;
    case TowerState::BLAST:
        os << "BLAST";
        break;
    case TowerState::DEAD:
        os << "DEAD";
        break;
    }

    return os;
}

struct _Actor {
    int64_t id;
    int64_t hp;
    DoubleVec2D position;

    _Actor() : id(0), hp(100), position(DoubleVec2D{0, 0}){};
};

struct _Blaster {
    bool blasting;
    int64_t impact_radius;

    _Blaster() : blasting(false), impact_radius(0) {}

    void blast() { blasting = true; }
    void reset() { blasting = false; }
    void setImpact(int64_t p_impact_radius) { impact_radius = p_impact_radius; }
};

struct _Unit : _Actor {
    DoubleVec2D destination;
    size_t speed;

    virtual void reset() { destination = DoubleVec2D::null; }
    void move(DoubleVec2D p_destination) {
        reset();
        destination = p_destination;
    }

    _Unit()
        : _Actor(), destination(DoubleVec2D::null),
          speed(Constants::Actor::BOT_SPEED){};
};

struct Bot : _Unit, _Blaster {
    BotState state;
    // move and blast at set destination
    DoubleVec2D final_destination;

    // move and transform at set destination
    DoubleVec2D transform_destination;
    bool transforming;

    void reset() override {
        transform_destination = DoubleVec2D::null;
        final_destination = DoubleVec2D::null;
        transforming = false;
        _Unit::reset();
        _Blaster::reset();
    }

    void blast_bot() {
        reset();
        blast();
    }

    // move to a target position and blast
    void blast_bot(DoubleVec2D target_position) {
        reset();
        if (target_position == position) {
            blast();
        } else {
            final_destination = target_position;
        }
    }

    void transform_bot() {
        reset();
        transforming = true;
    }

    void transform_bot(DoubleVec2D target_position) {
        reset();
        if (target_position == position) {
            transforming = true;
        } else {
            transform_destination = target_position;
        }
    }

    void copy(const Bot &reference_bot) {
        this->destination = reference_bot.destination;
        this->transform_destination = reference_bot.transform_destination;
        this->final_destination = reference_bot.final_destination;
        this->transforming = reference_bot.transforming;
        this->blasting = reference_bot.blasting;
    }

    Bot()
        : _Unit(), _Blaster(), state(BotState::IDLE),
          final_destination(DoubleVec2D::null),
          transform_destination(DoubleVec2D::null), transforming(false){};
};

inline std::ostream &operator<<(std::ostream &os, const Bot &bot) {
    using std::endl;
    os << "Bot(id: " << bot.id << ") {" << endl;
    os << "   hp: " << bot.hp << endl;
    os << "   position: " << bot.position << endl;
    os << "   state: " << bot.state << endl;
    os << "}" << endl;

    return os;
}

struct Tower : _Actor, _Blaster {
    TowerState state;

    void blast_tower() { blast(); }

    void copy(const Tower &reference_tower) {
        this->blasting = reference_tower.blasting;
    }

    Tower() : _Actor::_Actor(), _Blaster(), state(TowerState::IDLE){};
};

inline std::ostream &operator<<(std::ostream &os, Tower tower) {
    using std::endl;
    os << "Tower(id: " << tower.id << ") {" << endl;
    os << "   hp: " << tower.hp << endl;
    os << "   position: " << tower.position << endl;
    os << "   state: " << tower.state << endl;
    os << "}" << endl;

    return os;
}

struct MapElement {
    TerrainType type;
    void setTerrain(TerrainType p_type) { type = p_type; }
    TerrainType getTerrain() { return type; }
};

/**
 * Main Player state, the struct interface available to each player.
 */
struct State {
    std::array<std::array<MapElement, Constants::Map::MAP_SIZE>,
               Constants::Map::MAP_SIZE>
        map;

    std::vector<Bot> bots;
    std::vector<Bot> enemy_bots;

    int64_t num_bots;
    int64_t num_enemy_bots;

    std::vector<Tower> towers;
    std::vector<Tower> enemy_towers;

    int64_t num_towers;
    int64_t num_enemy_towers;

    int64_t score;

    State()
        : map(), bots(Constants::Actor::MAX_NUM_BOTS),
          enemy_bots(Constants::Actor::MAX_NUM_BOTS),
          num_bots(Constants::Actor::MAX_NUM_BOTS),
          num_enemy_bots(Constants::Actor::MAX_NUM_BOTS),
          towers(Constants::Map::MAP_SIZE * Constants::Map::MAP_SIZE),
          enemy_towers(Constants::Map::MAP_SIZE * Constants::Map::MAP_SIZE),
          num_towers(Constants::Map::MAP_SIZE * Constants::Map::MAP_SIZE),
          num_enemy_towers(Constants::Map::MAP_SIZE * Constants::Map::MAP_SIZE),
          score(0) {}
};

inline std::ostream &operator<<(std::ostream &os, const State &state) {
    using std::endl;

    os << "Map:" << endl;
    for (auto const &row : state.map) {
        for (auto const &elem : row) {
            switch (elem.type) {
            case TerrainType::LAND:
                os << "L ";
                break;
            case TerrainType::WATER:
                os << "W ";
                break;
            case TerrainType::FLAG:
                os << "F ";
                break;
            case TerrainType::TOWER:
                os << "T ";
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

    os << "-- Enemy Towers --" << endl;
    for (auto const &enemy_tower : state.enemy_towers) {
        os << enemy_tower << endl;
    }

    os << "-- Score --" << endl;
    os << state.score << endl;

    return os;
}

} // namespace player_state
