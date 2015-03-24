#ifndef MODEL_SERVER_UTIL_H
#define MODEL_SERVER_UTIL_H

#include <unistd.h>

namespace util {

static inline int cpu_cnt() {
    auto count = std::thread::hardware_concurrency();
    if (count) {
        return int { count };
    }

    return sysconf(_SC_NPROCESSORS_ONLN);
}

}

#endif // MODEL_SERVER_UTIL_H
