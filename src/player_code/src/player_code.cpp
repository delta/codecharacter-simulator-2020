/**
 * @file player_code.cpp
 * Defines player AI code
 */

#include "player_code/player_code.h"

namespace player_code {

using namespace player_state;

State PlayerCode::update(State state) {
    static int64_t num_turn = 0;
    ++num_turn;
    uint64_t num_state_bots = state.bots.size(),
             num_flags = state.flag_offsets.size();
    uint64_t bot_count = 0, flag_count = 0;

    // Assigning a quarter of them to move into the center flag
    for (; bot_count < num_state_bots / 4; ++bot_count) {
        auto &bot = state.bots[bot_count];
        auto flag_position = state.flag_offsets[flag_count++];
        flag_count %= num_flags;
        bot.move(flag_position);
    }

    // Assinging another quarter to move ahead of the flag locations and create
    // towers
    if (num_turn % 100 == 0) {
        for (; bot_count < num_state_bots / 2; ++bot_count) {
            auto &bot = state.bots[bot_count];
            auto flag_position = state.flag_offsets[(flag_count++) % num_flags];
            auto nearest_buildable_position =
                findNearestFreePosition(state, flag_position);
            // logr << "Transforming bot\n";
            bot.transform(nearest_buildable_position);
        }
    } else {
        // Going to enemy base and blasting
        for (; bot_count < num_state_bots / 2; ++bot_count) {
            auto &bot = state.bots[bot_count];
            bot.blast(Constants::Map::PLAYER2_BASE_POSITION);
        }
    }

    // Assigning half of the bots to create towers near enemy base location
    if (num_turn % 100 == 0) {
        for (; bot_count < num_state_bots; ++bot_count) {
            auto &bot = state.bots[bot_count];
            auto enemy_base = Constants::Map::PLAYER2_BASE_POSITION;
            auto nearest_buildable_position =
                findNearestFreePosition(state, enemy_base);
            bot.transform(nearest_buildable_position);
        }
    } else {
        // Try to transform in any flag
        for (; bot_count < num_state_bots; ++bot_count) {
            auto &bot = state.bots[bot_count];
            auto flag_position = state.flag_offsets[flag_count++];
            flag_count %= num_flags;
            bot.move(flag_position);
        }
    }

    // Checking if enemy bots are in range and blasting
    uint64_t tower_count = state.towers.size();

    for (uint64_t tower_index = 0; tower_index < tower_count; ++tower_index) {
        // Finding total number of enemy units in range
        uint64_t enemy_actors = 0;
        auto &tower = state.towers[tower_index];
        uint64_t tower_range = tower.impact_radius;

        for (int64_t enemy_bot_index = 0;
             enemy_bot_index < state.enemy_bots.size(); ++enemy_bot_index) {
            auto &bot = state.enemy_bots[enemy_bot_index];
            if (tower.position.distance(bot.position) <= tower_range) {
                ++enemy_actors;
            }
        }

        for (int64_t enemy_tower_index = 0;
             enemy_tower_index < state.enemy_towers.size();
             ++enemy_tower_index) {
            auto &enemy_tower = state.enemy_towers[enemy_tower_index];
            if (tower.position.distance(enemy_tower.position) <= tower_range) {
                ++enemy_actors;
            }
        }

        if (enemy_actors >= 0) {
            // logr << "Tower is blasting\n";
            tower.blast();
        }
    }

    return state;
}

} // namespace player_code
