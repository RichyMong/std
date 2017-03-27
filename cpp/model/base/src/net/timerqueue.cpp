#include <system_error>
#include <algorithm>
#include <sys/timerfd.h>
#include "timerqueue.h"

namespace net {

TimerQueue::TimerQueue()
{
    timerfd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd_ == -1) {
        throw std::system_error(errno, std::generic_category());
    }
}

bool TimerQueue::add_timer(Timer* timer)
{
    auto it = std::find(timers_.begin(), timers_.end(), timer);
    if (it == timers_.end()) {
        timers_.push_back(timer);
    }

    return it == timers_.end();
}

void TimerQueue::del_timer(Timer* timer)
{
    std::remove(timers_.begin(), timers_.end(), timer);
}

}
