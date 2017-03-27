# pragma once

#include <functional>
#include <chrono>

namespace net {

class Timer {
public:
    using TimerCallBack = std::function<void(Timer*)>;
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

    Timer(TimePoint when, TimerCallBack cb);

    template <typename T, class Period = std::ratio<1> >
    Timer(std::chrono::duration<T, Period> period, TimerCallBack cb, bool once = false);

    Timer(const Timer&) = delete;

    TimePoint when() const { return when_; }
    bool once() const { return once_; }

    void timeout();

private:
    std::chrono::steady_clock::duration period_;
    TimePoint when_;
    TimerCallBack cb_;
    bool once_;
};

}

