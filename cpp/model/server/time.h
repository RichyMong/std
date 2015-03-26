#ifndef MODEL_SERVER_TIME_H
#define MODEL_SERVER_TIME_H

#include "util.h"
#include <stdexcept>
#include <time.h>

namespace util {

class Time {
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

private:
    Time() : curr_msec_ {0} {
        update();
        if (curr_msec_ == 0) {
            throw std::runtime_error("cannot get current time");
        }

    }

    Time(const Time&);

    int64_t curr_msec_;
    char    log_time_[128];
};

}

#define current_msec Time::instance().current_msec()

#define cached_log_time Time::instance().log_time()

#endif // MODEL_SERVER_TIME_H
