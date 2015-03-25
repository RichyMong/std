#ifndef MODEL_SERVER_UTIL_H
#define MODEL_SERVER_UTIL_H

#include <thread>
#include <unistd.h>

namespace util {

static inline int cpu_cnt() {
    auto count = std::thread::hardware_concurrency();
    if (count) {
        return static_cast<int>(count);
    }

    return sysconf(_SC_NPROCESSORS_ONLN);
}

template <class T>
class Singleston {
public:
    static T& instance() {
        static T inst;
        return inst;
    }

protected:
    Singleston() { }
};

}

#endif // MODEL_SERVER_UTIL_H
