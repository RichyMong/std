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

    if ((curr_msec_ + kTimerResolution) > msec) {
        return;
    }

    curr_msec_ = msec;

    time_t secs = ts.tv_sec;
    tm result;
    localtime_r(&secs, &result);

    snprintf(log_time_, sizeof(log_time_), "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
             result.tm_year + 1900, result.tm_mon + 1, result.tm_mday,
             result.tm_hour, result.tm_min, result.tm_sec, ts.tv_nsec / 1000);
}

void Time::on_readable(Multiplex&) {
    // we don't need to read the ticks since we know the expired time point.
    expire_timer();
}

void Time::add_timer(const TimerObjPtr& tsp) {
    auto old_timer = timers_.empty() ? 0 : (*timers_.begin())->timepoint();
    timers_.insert(tsp);
    auto new_timer = (*timers_.begin())->timepoint();

    // avoid a system call if the difference is between 250ms.
    if (old_timer != new_timer &&
        std::abs(old_timer - new_timer) > Time::kTimerResolution) {
        prepare_timer();
    }
}

void Time::del_timer(const TimerObjPtr& tsp) {
    auto old_timer = timers_.empty() ? 0 : (*timers_.begin())->timepoint();
    timers_.erase(tsp);
    auto new_timer = timers_.empty() ? 0 : (*timers_.begin())->timepoint();

    if (old_timer != new_timer &&
        std::abs(old_timer - new_timer) > Time::kTimerResolution) {
        prepare_timer();
    }
}

void Time::prepare_timer() {
    auto timer = timers_.cbegin();
    if (timer != timers_.cend()) {
        set_timer((*timer)->timepoint() - curr_msec_);
    } else {
        set_timer(0);
    }
}

void Time::expire_timer() {
    auto size = timers_.size();

    for (auto it = timers_.begin(); it != timers_.end(); it = timers_.erase(it)) {
        if ((*it)->timepoint() > current_msec + Time::kTimerResolution) {
            break;
        }
        (*it)->timeout();
    }

    if (size != timers_.size()) {
        prepare_timer();
    }
}

void Time::set_timer(int msec) {
    itimerspec tvs;

    tvs.it_value.tv_sec = msec / 1000;
    tvs.it_value.tv_nsec = (msec % 1000) * 1000000;
    tvs.it_interval.tv_sec = 0;
    tvs.it_interval.tv_nsec = 0;

    if (timerfd_settime(timefd_, 0, &tvs, NULL) < 0) {
        fprintf(stderr, "timertimefd_settime: %s %d\n", strerror(errno), msec);
    }
}

}
