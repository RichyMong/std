#include "timer.h"
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>

namespace util {

bool timer_cmp(const TimerObjPtr a, const TimerObjPtr b) {
    return a->timepoint() < b->timepoint();
}

Timer::Timer(int msec)
    : timers_ { timer_cmp } {
    timefd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timefd_ < 0) {
        throw std::runtime_error("cannot create timer");
    }
    reset(msec);
}

void Timer::reset(int msec) {
    itimerspec tvs;

    tvs.it_interval.tv_sec = msec / 1000;
    tvs.it_interval.tv_nsec = (msec % 1000) * 1000000;
    tvs.it_value.tv_sec = msec / 1000;
    tvs.it_value.tv_nsec = (msec % 1000) * 1000000;

    if (timerfd_settime(timefd_, 0, &tvs, NULL) < 0) {
        std::perror("timertimefd_settime");
    }

    interval_msec_ = msec;
}

void Timer::on_readable(Multiplex&, FileObj*) {
    uint64_t ticks;
    auto nread = read(timefd_, &ticks, sizeof(ticks));
    if (nread == sizeof(ticks)) {
        expire_timer();
    } else if (nread < 0) {
        std::perror("read timer");
    } else {
        printf("invalid read size\n");
    }
}

void Timer::expire_timer() {
    printf("timepoint %ld\n", timers_.size());

    for (auto it = timers_.begin(); it != timers_.end(); it = timers_.erase(it)) {
        if ((*it)->timepoint() > current_msec) {
            break;
        }
        (*it)->timeout();
    }
}

}
