/**
 * @file timer.cpp
 * Defines timer member functions
 */

#include "drivers/timer.h"
#include "drivers/game_result.h"

namespace drivers {

Timer::Timer()
    : is_running(false), stopped(false), curr_duration(0),
      timer_wake_up_duration(10) {}

bool Timer::start(Interval total_timer_duration, const Callback &callback) {
    if (this->is_running) {
        return false;
    }

    this->is_running = true;
    this->curr_duration = Interval(0);

    std::thread(
        [this](Interval total_timer_duration, const Callback &callback) {
            while (this->curr_duration < total_timer_duration &&
                   !this->stopped) {
                std::this_thread::sleep_for(this->timer_wake_up_duration);
                this->curr_duration += this->timer_wake_up_duration;
            }
            this->is_running = false;
            if (!this->stopped)
                callback();
        },
        total_timer_duration, callback)
        .detach();

    return true;
}

void Timer::stop() {
    this->stopped = true;
    while (this->is_running)
        ;
    this->stopped = false;
}
} // namespace drivers
