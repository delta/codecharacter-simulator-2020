#include "state/player_state.h"

namespace player_state {
// Adding player state helper functions

ostream &operator<<(ostream &os,
                    array<array<MapElement, MAP_SIZE>, MAP_SIZE> &map) {
    os << "\n MAP \n";
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            auto type = map[x][y].getTerrain();
            switch (type) {
            case TerrainType::FLAG:
                os << "F";
                break;
            case TerrainType::LAND:
                os << "L";
                break;
            case TerrainType::WATER:
                os << "W";
                break;
            case TerrainType::TOWER:
                os << "T";
                break;
            }
        }
        os << "\n";
    }
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

Vec2D findNearestFlagLocation(array<array<MapElement, MAP_SIZE>, MAP_SIZE> map,
                              Vec2D position) {
    // Creating a queue to implement BFS
    queue<Vec2D> visit_next;
    visit_next.push(position);

    // Creating a visited array to not revisit the same position twice
    array<array<bool, MAP_SIZE>, MAP_SIZE> visited;

    for (size_t x = 0; x < MAP_SIZE; ++x) {
        for (size_t y = 0; y < MAP_SIZE; ++y) {
            visited[x][y] = false;
        }
    }

    // A helper function to check if positions are within the map
    size_t map_size = MAP_SIZE;
    auto is_position_valid = [map_size](Vec2D position) {
        return (position.x >= 0 && position.y >= 0 && position.x < map_size &&
                position.y < map_size);
    };

    vector<pair<int, int>> delta_changes = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!visit_next.empty()) {
        Vec2D position = visit_next.front();
        visit_next.pop();

        if (map[position.x][position.y].getTerrain() == TerrainType::FLAG) {
            return position;
        }

        visited[position.x][position.y] = true;

        // Adding neighbours of position which haven't already been visited
        for (const auto &delta_change : delta_changes) {
            auto new_x = position.x + delta_change.first;
            auto new_y = position.y + delta_change.second;
            Vec2D new_position = Vec2D(new_x, new_y);
            if (is_position_valid(new_position) && !visited[new_x][new_y]) {
                visit_next.push(new_position);
            }
        }
    }

    return Vec2D::null;
}

Vec2D findNearestBuildableOffset(
    array<array<MapElement, MAP_SIZE>, MAP_SIZE> map, Vec2D position) {
    // Creating a queue to implement BFS
    queue<Vec2D> visit_next;
    visit_next.push(position);

    // Creating a visited array to not revisit the same position twice
    array<array<bool, MAP_SIZE>, MAP_SIZE> visited;

    for (size_t x = 0; x < MAP_SIZE; ++x) {
        for (size_t y = 0; y < MAP_SIZE; ++y) {
            visited[x][y] = false;
        }
    }

    // A helper function to check if positions are within the map
    size_t map_size = MAP_SIZE;
    auto is_position_valid = [map_size](Vec2D position) {
        return (position.x >= 0 && position.y >= 0 && position.x < map_size &&
                position.y < map_size);
    };

    vector<pair<int, int>> delta_changes = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!visit_next.empty()) {
        Vec2D position = visit_next.front();
        visit_next.pop();
        TerrainType terrain = map[position.x][position.y].getTerrain();

        if (terrain == TerrainType::FLAG || terrain == TerrainType::LAND) {
            return position;
        }

        visited[position.x][position.y] = true;

        // Adding neighbours of position which haven't already been visited
        for (const auto &delta_change : delta_changes) {
            auto new_x = position.x + delta_change.first;
            auto new_y = position.y + delta_change.second;
            Vec2D new_position = Vec2D(new_x, new_y);
            if (is_position_valid(new_position) && !visited[new_x][new_y]) {
                visit_next.push(new_position);
            }
        }
    }

    return Vec2D::null;
}

} // namespace player_state
