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

    if (turn_count == 1) {
        int count = 0;
        for (auto &bot : state.bots) {
            bot.blast_bot({(double) count + 1, 0});
            count++;
        }
    }

    logr << "Bots:" << endl;
    for (auto &bot : state.bots) {
        if (bot.position != DoubleVec2D{0.5, 0.5}) {
            logr << bot.id << " " << bot.position << " " << bot.state << endl;
        }
    }

    logr << "Towers:" << endl;
    for (auto &tower : state.towers) {
        logr << tower.id << " " << tower.position << " " << tower.state << endl;
    }

    return state;
}

} // namespace player_code
