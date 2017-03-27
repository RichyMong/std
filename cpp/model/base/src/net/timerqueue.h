# pragma once

#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include "fileobj.h"

namespace net {

using timerid_t = int;
using TimerCallBack = std::function<void(timerid_t)>;
using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

static constexpr timerid_t INVALID_TIMER = 0;

class TimerQueue {
public:
    TimerQueue();

    timerid_t add_timer(TimePoint when, TimerCallBack callback);

    template <typename T, class Period = std::ratio<1> >
    timerid_t add_timer(std::chrono::duration<T, Period> period,
                       TimerCallBack callback,
                       bool once = false);

    void del_timer(timerid_t id);

private:
    class TimerQueueImpl;
    std::unique_ptr<TimerQueueImpl> tq_impl_;
};

}
