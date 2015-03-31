#ifndef MODEL_SERVER_TIMER_H
#define MODEL_SERVER_TIMER_H

#include "util.h"
#include "time.h"
#include "epoll.h"
#include <set>
#include <memory>

namespace util {

class Timer : public FileObj {
public:
    explicit Timer(int msec);

    int getfd() const { return timefd_; }

    void on_readable(Multiplex&);

    void reset(int msec);

    int next_timer() {
        auto it = timers_.cbegin();
        return (it != timers_.cend())
                ? (*it)->timepoint() - current_msec
                : -1;
    }

    void add_timer(TimerObjPtr sp) {
        timers_.insert(sp);
    }

    void del_timer(TimerObjPtr sp) {
        timers_.erase(sp);
    }

private:
    void expire_timer();

    int  timefd_;
    int  interval_msec_;

    std::multiset<TimerObjPtr, bool(*)(const TimerObjPtr, const TimerObjPtr)> timers_;
};

}

#endif // MODEL_SERVER_TIMER_H
