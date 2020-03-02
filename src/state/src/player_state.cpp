#include "state/player_state.h"

namespace player_state {

// Assinging the null values
Bot Bot::null = {-1};
Tower Tower::null = {-1};

// Adding player state helper functions
ostream &operator<<(ostream &os,
                    const array<array<MapElement, MAP_SIZE>, MAP_SIZE> &map) {
    os << "Map {";
    for (size_t x = 0; x < MAP_SIZE; ++x) {
        os << "{";
        for (size_t y = 0; y < MAP_SIZE; ++y) {
            auto type = map[x][y].getTerrain();
            switch (type) {
            case TerrainType::FLAG:
                os << "F, ";
                break;
            case TerrainType::LAND:
                os << "L, ";
                break;
            case TerrainType::WATER:
                os << "W, ";
                break;
            case TerrainType::TOWER:
                os << "T, ";
                break;
            }
        }
        os << "}\n";
    }
    os << "}";

    return os;
}

ostream &operator<<(ostream &os, const TowerState &tower_state) {
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

ostream &operator<<(ostream &os, const Tower &tower) {
    os << "Tower(id: " << tower.id << ") {" << endl;
    os << "   hp: " << tower.hp << endl;
    os << "   state: " << tower.state << endl;
    os << "}" << endl;
    return os;
}

ostream &operator<<(ostream &os, const Bot &bot) {
    os << "Bot(id: " << bot.id << ") {" << endl;
    os << "   hp: " << bot.hp << endl;
    os << "   position: " << bot.position << endl;
    os << "   state: " << bot.state << endl;
    os << "}" << endl;
    return os;
}

ostream &operator<<(ostream &os, const BotState &bot_state) {
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

ostream &operator<<(ostream &os, const State &state) {
    os << "Map:" << endl;
    os << state.map << endl;

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
    os << "{" << state.scores[0] << ", " << state.scores[1] << endl;

    return os;
}

array<array<uint64_t, MAP_SIZE>, MAP_SIZE> getActorCounts(const State &state) {
    array<array<uint64_t, MAP_SIZE>, MAP_SIZE> actor_counts{};

    for (auto bot : state.bots) {
        DoubleVec2D bot_position = bot.position;
        actor_counts[std::floor(bot_position.x)][std::floor(bot_position.y)]++;
    }

    for (auto enemy_bot : state.enemy_bots) {
        DoubleVec2D enemy_bot_position = enemy_bot.position;
        actor_counts[std::floor(enemy_bot_position.x)]
                    [std::floor(enemy_bot_position.y)]++;
    }

    for (auto tower : state.towers) {
        DoubleVec2D tower_position = tower.position;
        actor_counts[std::floor(tower_position.x)]
                    [std::floor(tower_position.y)]++;
    }

    for (auto enemy_tower : state.enemy_towers) {
        DoubleVec2D enemy_tower_position = enemy_tower.position;
        actor_counts[std::floor(enemy_tower_position.x)]
                    [std::floor(enemy_tower_position.y)]++;
    }

    return actor_counts;
}

Bot &getBotById(State &state, int64_t bot_id) {
    for (auto &bot : state.bots) {
        if (bot.id == bot_id) {
            return bot;
        }
    }

    for (auto &bot : state.enemy_bots) {
        if (bot.id == bot_id) {
            return bot;
        }
    }

    return Bot::null;
}

Tower &getTowerById(State &state, int64_t tower_id) {
    for (auto &tower : state.towers) {
        if (tower.id == tower_id) {
            return tower;
        }
    }

    for (auto &tower : state.enemy_towers) {
        if (tower.id == tower_id) {
            return tower;
        }
    }

    return Tower::null;
}

DoubleVec2D findNearestFlagPosition(const State &state, DoubleVec2D position) {
    // The nearest flag location only depends upon the terrain type being flag
    // and not the position count
    auto is_flag = [](TerrainType terrain, uint64_t position_count) -> bool {
        return (terrain == TerrainType::FLAG);
    };

    Vec2D position_offset =
        Vec2D(std::floor(position.x), std::floor(position.y));
    Vec2D nearest_offset = findNearestOffset(state, position_offset, is_flag);
    if (nearest_offset == Vec2D::null) {
        return DoubleVec2D::null;
    }
    DoubleVec2D center_position =
        DoubleVec2D(nearest_offset.x + 0.5, nearest_offset.y + 0.5);
    return center_position;
}

DoubleVec2D findNearestFreePosition(const State &state, DoubleVec2D position) {
    // For a position to be buildable, we need both the terrain to be of type
    // FLAG or LAND and the position should be occupied by on actors
    auto is_buildable = [](TerrainType terrain,
                           uint64_t position_count) -> bool {
        return (
            (terrain == TerrainType::FLAG || terrain == TerrainType::LAND) &&
            position_count == 0);
    };

    Vec2D position_offset =
        Vec2D(std::floor(position.x), std::floor(position.y));
    Vec2D nearest_offset =
        findNearestOffset(state, position_offset, is_buildable);
    if (nearest_offset == Vec2D::null) {
        return DoubleVec2D::null;
    }

    DoubleVec2D center_position =
        DoubleVec2D(nearest_offset.x + 0.5, nearest_offset.y + 0.5);
    return center_position;
}

Vec2D findNearestOffset(
    const State &state, Vec2D position,
    std::function<bool(TerrainType terrain, uint64_t position_count)>
        match_position) {
    // Creating a visited array to not revisit the same position twice
    array<array<bool, MAP_SIZE>, MAP_SIZE> visited;

    for (size_t x = 0; x < MAP_SIZE; ++x) {
        for (size_t y = 0; y < MAP_SIZE; ++y) {
            visited[x][y] = false;
        }
    }

    // Creating a count array of each actor in the map
    auto actor_counts = getActorCounts(state);

    // A helper function to check if positions are within the map
    long map_size = MAP_SIZE;
    auto position_valid = [map_size](Vec2D position) {
        return (position.x >= 0 && position.y >= 0 && position.x < map_size &&
                position.y < map_size);
    };

    // If the given position is outside the map, we simply return Vec2D::NULL
    if (!position_valid(position)) {
        return Vec2D::null;
    }

    // Storing all directions in which the position can consider as neighbours
    // of unit distance
    vector<pair<int, int>> delta_changes = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // Creating a queue to implement BFS
    auto &map = state.map;
    queue<Vec2D> visit_next;
    visit_next.push(position);
    visited[position.x][position.y] = true;

    while (!visit_next.empty()) {
        Vec2D position = visit_next.front();
        visit_next.pop();

        TerrainType terrain = map[position.x][position.y].getTerrain();

        if (match_position(terrain, actor_counts[position.x][position.y])) {
            return position;
        }

        // Adding neighbours of position which haven't already been visited
        for (const auto &delta_change : delta_changes) {
            auto new_x = position.x + delta_change.first;
            auto new_y = position.y + delta_change.second;
            Vec2D new_position = Vec2D(new_x, new_y);
            if (position_valid(new_position) && !visited[new_x][new_y]) {
                visit_next.push(new_position);
                visited[new_x][new_y] = true;
            }
        }
    }

    return Vec2D::null;
}

} // namespace player_state
