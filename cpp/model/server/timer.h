#ifndef MODEL_SERVER_TIMER_H
#define MODEL_SERVER_TIMER_H

#include "util.h"
#include "time.h"
#include "epoll.h"
#include <set>
#include <memory>

namespace util {

class TimerObj {
public:
    virtual void timeout() = 0;

    int64_t timepoint() const {
        return timepoint_msec_;
    }

    void update() {
        timepoint_msec_ += 2000;
    }

protected:
    explicit TimerObj(int msec)
        : timepoint_msec_ {msec + current_msec} {
    }

    int64_t timepoint_msec_;
};

class Timer : public FileObj {
public:
    explicit Timer(int msec);

    int getfd() const { return timefd_; }

    void on_readable(Multiplex& mutiplex);

    void reset(int msec);

    void add_timer(TimerObj* sp) {
        timers_.insert(sp);
    }

    void del_timer(TimerObj* sp) {
        timers_.erase(sp);
    }

private:
    void expire_timer();

    int  timefd_;
    int  interval_msec_;

    std::set<TimerObj*, bool(*)(const TimerObj*, const TimerObj*)> timers_;
};

}

#endif // MODEL_SERVER_TIMER_H
