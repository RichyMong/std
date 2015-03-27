#include "time.h"
#include <time.h>

namespace util {

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

}
