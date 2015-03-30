#ifndef MODEL_SERVER_TIME_H
#define MODEL_SERVER_TIME_H

#include "util.h"
#include "epoll.h"
#include <set>
#include <stdexcept>
#include <time.h>

namespace util {

class TimerObj;
typedef std::shared_ptr<TimerObj> TimerObjPtr;

class Time : public FileObj {
public:
    int64_t current_msec() const {
        return curr_msec_;
    }

    void update();

    const char* log_time() { return log_time_; }

    static Time& instance() {
        static thread_local Time the_time;
        return the_time;
    }

    int next_timer();

    void add_timer(TimerObjPtr sp);

    void del_timer(TimerObjPtr sp) {
        timers_.erase(sp);
    }

    void expire_timer();

    int getfd() const { return timefd_; }

    void on_readable(Multiplex&, FileObj*);

private:
    Time();

    Time(const Time&);

    void set_timer(int msec);

private:
    int64_t curr_msec_;
    char    log_time_[128];

    int     timefd_;

    std::multiset<TimerObjPtr, bool(*)(const TimerObjPtr, const TimerObjPtr)> timers_;
};

class TimerObj {
public:
    virtual void timeout() = 0;

    int64_t timepoint() const {
        return timepoint_msec_;
    }

protected:
    explicit TimerObj(int msec)
        : timepoint_msec_ { msec + Time::instance().current_msec() } {
    }

    int64_t timepoint_msec_;
};

}

#define the_time util::Time::instance()

#define current_msec util::Time::instance().current_msec()

#define cached_log_time util::Time::instance().log_time()

#endif // MODEL_SERVER_TIME_H
