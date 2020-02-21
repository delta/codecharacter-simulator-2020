/**
 * @file timer.cpp
 * Defines timer member functions
 */

#include "drivers/timer.h"
#include "drivers/game_result.h"

namespace drivers {

Timer::Timer()
    : is_running(false), stop_timer(false), timer_run_duration(0),
      timer_wake_up_duration(10) {}

bool Timer::start(Interval total_timer_duration, const Callback &callback) {
    if (this->is_running) {
        return false;
    }

    this->is_running = true;
    this->timer_run_duration = Interval(0);

    std::thread(
        [this](Interval total_timer_duration, const Callback &callback) {
            while (this->timer_run_duration < total_timer_duration &&
                   !this->stop_timer) {
                std::this_thread::sleep_for(this->timer_wake_up_duration);
                this->timer_run_duration += this->timer_wake_up_duration;
            }
            if (!this->stop_timer)
                callback();
            this->is_running = false;
        },
        total_timer_duration, callback)
        .detach();

    return true;
}

void Timer::stop() {
    this->stop_timer = true;
    while (this->is_running)
        ;
    this->stop_timer = false;
}
} // namespace drivers
