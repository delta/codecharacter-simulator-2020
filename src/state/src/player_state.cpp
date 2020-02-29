#include "state/player_state.h"

namespace player_state {
// Adding player state helper functions

void Print(array<array<MapElement, MAP_SIZE>, MAP_SIZE> map) {
    std::cerr << "\n PRINTING MAP \n";
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            switch (map[x][y].getTerrain()) {
            case TerrainType::FLAG:
                std::cerr << "F";
                break;
            case TerrainType::LAND:
                std::cerr << "L";
                break;
            case TerrainType::WATER:
                std::cerr << "W";
                break;
            case TerrainType::TOWER:
                std::cerr << "T";
                break;
            }
        }
        std::cerr << "\n";
    }
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
