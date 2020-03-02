/**
 * @file player_code.cpp
 * Defines player AI code
 */

#include "player_code/player_code.h"

namespace player_code {

using namespace player_state;

State PlayerCode::update(State state) {
    static int turn_count = 0;
    turn_count++;

    // cout << "Turn count :\t" << turn_count << "\n";
    logr << state.bots.size() << " \t" << state.towers.size() << "\n";

    if (turn_count == 1) {
        state.bots[0].transform_bot(DoubleVec2D(5, 5));
        state.bots[1].transform_bot(DoubleVec2D(10, 10));
    }

    if (turn_count == 10) {
        state.towers[0].blast_tower();
        state.towers[1].blast_tower();
    }

    return state;
}

} // namespace player_code
