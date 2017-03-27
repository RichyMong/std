# pragma once

#include <functional>
#include <chrono>

namespace net {

class Timer {
public:
    Timer(TimePoint when, TimerCallBack cb);

    template <typename T, class Period = std::ratio<1> >
    Timer(std::chrono::duration<T, Period> period, TimerCallBack cb, bool once = false);

    Timer(const Timer&) = delete;

    TimePoint when() const { return when_; }
    bool once() const { return once_; }
    int id() const { return id_; }
    void id(int new_id) { id_ = new_id; }

    void timeout();

private:
    std::chrono::steady_clock::duration period_;
    TimePoint when_;
    TimerCallBack cb_;
    timerid_t id_;
    bool once_;
};

}

