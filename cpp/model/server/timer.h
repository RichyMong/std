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

class Timer : public EventHandler {
public:
    explicit Timer(int msec);

    void on_readable(Multiplex& mutiplex);

    void on_writeable(Multiplex& mutiplex) { }

    void reset(int msec);

    void add_event(util::Multiplex& mutiplex) {
        mutiplex.add(timefd_, ARC_READ_EVENT, this);
    }

    void add_timer(TimerObj* sp) {
        timers_.insert(sp);
    }

    void del_timer(TimerObj* sp) {
        timers_.erase(sp);
    }

private:
    int timefd_;

    void expire_timer();

    int  interval_msec_;

    std::set<TimerObj*, bool(*)(const TimerObj*, const TimerObj*)> timers_;
};

}

#endif // MODEL_SERVER_TIMER_H
