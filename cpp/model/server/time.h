#ifndef MODEL_SERVER_TIME_H
#define MODEL_SERVER_TIME_H

#include "util.h"
#include <stdexcept>
#include <time.h>

namespace util {

class Time : public Singleston<Time> {
    friend class Singleston<Time>;
public:
    int64_t current_msec() const {
        return curr_msec_;
    }

    void update() {
        timespec ts;

        if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) == 0) {
            curr_msec_ = ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);
        }
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
};

}

#define current_msec Time::instance().current_msec()

#endif // MODEL_SERVER_TIME_H
