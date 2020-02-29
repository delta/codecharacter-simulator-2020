#include "drivers/timer.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace drivers;
using namespace std;

const int timer_duration = 100;
const int grace_period = 10;

TEST(TimerTest, NormalRunTest) {
    Timer timer;
    bool time_limit_exceeded = false;
    bool timer_started =
        timer.start((Timer::Interval(timer_duration)),
                    [&time_limit_exceeded] { time_limit_exceeded = true; });
    EXPECT_TRUE(timer_started);
    EXPECT_FALSE(time_limit_exceeded);

    this_thread::sleep_for(Timer::Interval(timer_duration + grace_period));
    EXPECT_TRUE(time_limit_exceeded);

    // Should work again
    time_limit_exceeded = false;

    timer_started =
        timer.start((Timer::Interval(timer_duration)),
                    [&time_limit_exceeded] { time_limit_exceeded = true; });
    EXPECT_TRUE(timer_started);

    this_thread::sleep_for(Timer::Interval(timer_duration + grace_period));
    EXPECT_TRUE(time_limit_exceeded);

    // Check for case when time limit is not exceeded
    time_limit_exceeded = false;

    timer_started =
        timer.start((Timer::Interval(timer_duration)),
                    [&time_limit_exceeded] { time_limit_exceeded = true; });
    EXPECT_TRUE(timer_started);

    this_thread::sleep_for(Timer::Interval(timer_duration - grace_period));
    EXPECT_FALSE(time_limit_exceeded);

    this_thread::sleep_for(Timer::Interval(2 * grace_period));
}

TEST(TimerTest, MultipleTimersInvalid) {
    Timer timer;
    int num_timers_running = 0;

    bool timer_started =
        timer.start((Timer::Interval(timer_duration)),
                    [&num_timers_running] { num_timers_running++; });
    EXPECT_TRUE(timer_started);

    // Can't start timer if it's already running
    timer_started =
        timer.start((Timer::Interval(timer_duration)),
                    [&num_timers_running] { num_timers_running++; });
    EXPECT_FALSE(timer_started);

    this_thread::sleep_for(Timer::Interval(timer_duration + grace_period));
    // Count should only have been incremented once
    EXPECT_EQ(num_timers_running, 1);
}

TEST(TimerTest, StopTest) {
    Timer timer;
    bool timer_completed = false;
    bool timer_started =
        timer.start((Timer::Interval(timer_duration)),
                    [&timer_completed] { timer_completed = true; });

    EXPECT_TRUE(timer_started);

    timer.stop();
    this_thread::sleep_for(Timer::Interval(timer_duration + grace_period));
    // Flag should have remain unset as timer was cancelled
    EXPECT_FALSE(timer_completed);
}
