# pragma once

#include <utility>

namespace util {

template <class T>
class SingleSton {
public:
    SingleSton() = default;

    static T& instance()
    {
        static T obj;
        return obj;
    }
    SingleSton(const SingleSton&) = delete;
    SingleSton& operator=(const SingleSton&) = delete;
};

}
