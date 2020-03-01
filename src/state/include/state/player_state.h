/**
 * @file player_state.h
 * Player structure for game operations
 */

#pragma once

#include "constants/constants.h"
#include "physics/vector.hpp"
#include "state/utilities.h"

#include <array>
#include <functional>
#include <queue>

using namespace std;
using namespace Constants::Map;

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

enum class TowerState : int8_t {
    // Tower is doing nothing
    IDLE,
    // Tower is blowing up.
    BLAST,
    // Tower is dead
    DEAD
};

struct _Actor {
    int64_t id;
    int64_t hp;
    DoubleVec2D position;

    _Actor() : id(0), hp(100), position(DoubleVec2D{0, 0}){};
    _Actor(int64_t id) : id(id), hp(100), position(DoubleVec2D{0, 0}){};
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

    _Unit(int64_t id)
        : _Actor(id), destination(DoubleVec2D::null),
          speed(Constants::Actor::BOT_SPEED) {}
};

struct Bot : _Unit, _Blaster {
    // A static member to represent a null bot or an invalid bot
    static Bot null;

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

    Bot(const Bot &reference_bot) {
        this->destination = reference_bot.destination;
        this->transform_destination = reference_bot.transform_destination;
        this->final_destination = reference_bot.final_destination;
        this->transforming = reference_bot.transforming;
        this->blasting = reference_bot.blasting;
        this->position = reference_bot.position;
        this->hp = reference_bot.hp;
        this->id = reference_bot.id;
        this->impact_radius = reference_bot.impact_radius;
        this->state = reference_bot.state;
    }

    bool operator==(const Bot &bot) const {
        return (bot.id == id && bot.state == state && bot.hp == hp &&
                bot.final_destination == final_destination &&
                bot.destination == destination &&
                bot.transform_destination == transform_destination &&
                bot.transforming == transforming && bot.blasting == blasting);
    }

    Bot()
        : _Unit(), _Blaster(), state(BotState::IDLE),
          final_destination(DoubleVec2D::null),
          transform_destination(DoubleVec2D::null), transforming(false){};

    Bot(int64_t id)
        : _Unit(id), _Blaster(), final_destination(DoubleVec2D::null),
          transform_destination(DoubleVec2D::null), transforming(false){};

    virtual ~Bot() {}
};

struct Tower : _Actor, _Blaster {
    static Tower null;

    TowerState state;

    void blast_tower() { blast(); }

    Tower(const Tower &reference_tower) {
        this->id = reference_tower.id;
        this->hp = reference_tower.hp;
        this->position = reference_tower.position;
        this->state = reference_tower.state;
        this->blasting = reference_tower.blasting;
    }

    bool operator==(const Tower &tower) const {
        return (tower.id == id && tower.hp == hp && tower.state == state &&
                tower.position == position && tower.blasting == blasting);
    }

    Tower() : _Actor(), _Blaster(), state(TowerState::IDLE){};
    Tower(int64_t id) : _Actor(id), _Blaster(), state(TowerState::IDLE){};
};

struct MapElement {
    TerrainType type;
    void setTerrain(TerrainType p_type) { type = p_type; }
    TerrainType getTerrain() { return type; }
};

/**
 * Main Player state, the struct interface available to each player.
 */
struct State {
    array<array<MapElement, Constants::Map::MAP_SIZE>, Constants::Map::MAP_SIZE>
        map;
    vector<Vec2D> flag_offsets;

    vector<Bot> bots;
    vector<Bot> enemy_bots;

    int64_t num_bots;
    int64_t num_enemy_bots;

    vector<Tower> towers;
    vector<Tower> enemy_towers;

    int64_t num_towers;
    int64_t num_enemy_towers;

    array<int64_t, 2> scores;

    State()
        : map(), bots(Constants::Actor::MAX_NUM_BOTS),
          enemy_bots(Constants::Actor::MAX_NUM_BOTS),
          num_bots(Constants::Actor::MAX_NUM_BOTS),
          num_enemy_bots(Constants::Actor::MAX_NUM_BOTS),
          towers(Constants::Actor::MAX_NUM_TOWERS),
          enemy_towers(Constants::Actor::MAX_NUM_TOWERS),
          num_towers(Constants::Actor::MAX_NUM_TOWERS),
          num_enemy_towers(Constants::Actor::MAX_NUM_TOWERS), scores({0, 0}) {}
};

// Defining function prototypes
Vec2D findNearestFlagOffset(array<array<MapElement, MAP_SIZE>, MAP_SIZE> map,
                            Vec2D position);

Vec2D findNearestBuildableOffset(
    array<array<MapElement, MAP_SIZE>, MAP_SIZE> map, Vec2D position);

Vec2D findNearestOffset(array<array<MapElement, MAP_SIZE>, MAP_SIZE> map,
                        Vec2D position, std::function<bool(TerrainType type)>);

Bot getBotById(State state, int64_t bot_id);

Tower getTowerById(State state, int64_t tower_id);

ostream &operator<<(ostream &os, const TowerState &tower_state);

ostream &operator<<(ostream &os, const Tower &tower);

ostream &operator<<(ostream &os, const Bot &bot);

ostream &operator<<(ostream &os, const BotState &bot_state);

ostream &operator<<(ostream &os, const State &state);

} // namespace player_state
