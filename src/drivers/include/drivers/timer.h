/**
 * @file timer.h
 * Declarations for a timer utility
 */

#pragma once

#include "drivers/drivers_export.h"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <thread>

namespace drivers {

/**
 * An asynchronous timer class
 */
class DRIVERS_EXPORT Timer {
  private:
    /**
     * True if this timer is running, false otherwise
     */
    std::atomic_bool is_running;

    /**
     * Temporary used by the stop method
     * Set to true by stop, and reset to false right before stop returns
     */
    std::atomic_bool stop_timer;

    /**
     * Amount of time the timer has been running for
     * Valid if is_running is true
     */
    std::chrono::milliseconds timer_run_duration;

    /**
     * Timer wakes up every timer_wake_up_duration ms to check if it has been
     * stopped
     */
    std::chrono::milliseconds timer_wake_up_duration;

  public:
    /**
     * Interval of time
     */
    typedef std::chrono::milliseconds Interval;

    /**
     * Callback that timer can call
     */
    typedef std::function<void(void)> Callback;

    /**
     * Constructor for Timer
     */
    Timer();

    Timer(Interval timer_wake_up_duration);

    /**
     * Starts this timer. Works only if is_running is false.
     * Spawns a separate thread for the timer
     *
     * @param[in]  total_timer_duration  The total timer run duration
     * @param[in]  callback              The callback when timer expires
     *
     * @return     false if is_running is true, else true
     */
    bool start(Interval total_timer_duration, const Callback &callback);

    /**
     * Method to stop the timer
     *
     * Stopped timer won't call callback unless it's very close to expiring
     * already.
     * Blocks for at most timer_wake_up_duration ms
     */
    void stop();
};
} // namespace drivers
