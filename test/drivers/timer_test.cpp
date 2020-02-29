#include "drivers/timer.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace drivers;
using namespace std;

const int timer_duration = 100;
const int grace_period = 50;

TEST(TimerTest, NormalRunTest) {
    Timer timer;
    bool flag = false;
    bool result;

    result = timer.start((Timer::Interval(timer_duration)),
                         [&flag] { flag = true; });
    EXPECT_EQ(result, true);
    EXPECT_EQ(flag, false);

    this_thread::sleep_for(Timer::Interval(timer_duration + grace_period));
    EXPECT_EQ(flag, true);

    // Should work again
    flag = false;

    result = timer.start((Timer::Interval(timer_duration)),
                         [&flag] { flag = true; });
    EXPECT_EQ(result, true);

    this_thread::sleep_for(Timer::Interval(timer_duration + grace_period));
    EXPECT_EQ(flag, true);
}

TEST(TimerTest, MultipleTimersInvalid) {
    Timer timer;
    int count = 0;
    bool result;

    result =
        timer.start((Timer::Interval(timer_duration)), [&count] { count++; });
    EXPECT_EQ(result, true);

    // Can't start timer if it's already running
    result =
        timer.start((Timer::Interval(timer_duration)), [&count] { count++; });
    EXPECT_EQ(result, false);

    this_thread::sleep_for(Timer::Interval(timer_duration + grace_period));
    // Count should only have been incremented once
    EXPECT_EQ(count, 1);
}

TEST(TimerTest, StopTest) {
    Timer timer;
    bool flag = false;
    bool result = timer.start((Timer::Interval(timer_duration)),
                              [&flag] { flag = true; });

    EXPECT_TRUE(result);

    timer.stop();
    this_thread::sleep_for(Timer::Interval(timer_duration + grace_period));
    // Flag should have remain unset as timer was cancelled
    EXPECT_FALSE(flag);
}
