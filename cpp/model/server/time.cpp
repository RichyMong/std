#include "time.h"
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>

namespace util {

static bool timer_cmp(const TimerObjPtr a, const TimerObjPtr b) {
    return a->timepoint() < b->timepoint();
}

Time::Time()
    : curr_msec_ {0}, timers_ { timer_cmp } {
    update();

    if (curr_msec_ == 0) {
        throw std::runtime_error("cannot get current time");
    }

    timefd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timefd_ < 0) {
        throw std::runtime_error("cannot create timer");
    }

    set_timer(0);
}

void Time::update() {
    timespec ts;

    auto msec = curr_msec_;
    if (clock_gettime(CLOCK_REALTIME_COARSE, &ts) == 0) {
        msec = ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
    }

    if (curr_msec_ == msec) {
        return;
    }

    curr_msec_ = msec;

    time_t secs = ts.tv_sec;
    tm result;
    localtime_r(&secs, &result);

    snprintf(log_time_, sizeof(log_time_), "%04d-%02d-%02d %02d:%02d:%02d[%06ld]",
             result.tm_year + 1900, result.tm_mon + 1, result.tm_mday,
             result.tm_hour, result.tm_min, result.tm_sec, ts.tv_nsec / 1000);
}

int Time::next_timer() {
    auto it = timers_.cbegin();

    return (it != timers_.cend()) ? (*it)->timepoint() - curr_msec_ : -1;
}

void Time::on_readable(Multiplex&, FileObj*) {
    uint64_t ticks;
    auto nread = read(timefd_, &ticks, sizeof(ticks));
    if (nread == sizeof(ticks)) {
        expire_timer();
    } else if (nread < 0) {
        std::perror("read timer");
    } else {
        printf("invalid read size\n");
    }

    auto it = timers_.cbegin();
    if (it != timers_.cend()) {
        set_timer((*it)->timepoint() - curr_msec_);
    } else {
        printf("disable the timer\n");
        set_timer(0);
    }
}

void Time::add_timer(TimerObjPtr sp) {
    auto empty = timers_.empty();
    timers_.insert(sp);
    if (empty) {
        set_timer(sp->timepoint() - curr_msec_);
    }
}


void Time::expire_timer() {
    printf("Time: timer count %ld\n", timers_.size());

    for (auto it = timers_.begin(); it != timers_.end(); it = timers_.erase(it)) {
        if ((*it)->timepoint() > current_msec) {
            break;
        }
        (*it)->timeout();
    }
}

void Time::set_timer(int msec) {
    itimerspec tvs;

    tvs.it_value.tv_sec = msec / 1000;
    tvs.it_value.tv_nsec = (msec % 1000) * 1000000;
    tvs.it_interval.tv_sec = 0;
    tvs.it_interval.tv_nsec = 0;

    if (timerfd_settime(timefd_, 0, &tvs, NULL) < 0) {
        std::perror("timertimefd_settime");
    }
}

}
