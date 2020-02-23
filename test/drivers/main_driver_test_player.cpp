#include "drivers/shared_memory_utils/shared_memory_player.h"
#include "drivers/timer.h"
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace drivers;

// Arg 1: shared_memory_name
// Arg 2: time_limit_ms
// Arg 3: num_turns
// Arg 4: turn_instruction_limit
int main(int argc, char *argv[]) {
    if (argc != 5) {
        return 1;
    }

    string shared_memory_name = string(argv[1]);
    int time_limit_ms = atoi(argv[2]);
    int num_turns = atoi(argv[3]);
    int turn_instruction_limit = atoi(argv[4]);

    // Create shm for player
    unique_ptr<SharedMemoryPlayer> shm_player =
        make_unique<SharedMemoryPlayer>(shared_memory_name);
    drivers::SharedBuffer *buf = shm_player->getBuffer();

    // Set time limit on player side
    Timer timer;
    atomic_bool is_time_over(false);
    timer.start((Timer::Interval(time_limit_ms)),
                [&is_time_over]() { is_time_over = true; });

    // Run player updates for num_turns
    for (int i = 0; i < num_turns && !is_time_over; ++i) {
        while (!buf->is_player_running && !is_time_over)
            ;

        if (i < num_turns / 2)
            buf->instruction_counter = turn_instruction_limit;
        else
            buf->instruction_counter = turn_instruction_limit + 1;

        buf->is_player_running = false;
    }

    return 0;
}
