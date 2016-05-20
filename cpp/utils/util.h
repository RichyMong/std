# pragma once

#include <chrono>
#include <type_traits>

namespace util {
    template <typename F, typename... Args>
    using ResultOf = typename std::result_of<F(Args...)>::type;

    using Duration = std::chrono::milliseconds;

    template<typename F, typename... Args> inline
    typename std::enable_if<std::is_void<ResultOf<F, Args...>>::value, Duration>::type
    timeit(F&& func, Args... args)
    {
        using namespace std::chrono;
        using Clock = std::chrono::steady_clock;
        auto start_tp = Clock::now();
        func(std::forward<Args>(args)...);
        return duration_cast<milliseconds>(Clock::now() - start_tp);
    }

    template<typename F, typename... Args> inline
    typename std::enable_if<!std::is_void<ResultOf<F, Args...>>::value,
        std::pair<ResultOf<F, Args...>, Duration>>::type
    timeit(F&& func, Args&&... args)
    {
        using namespace std::chrono;
        using Clock = std::chrono::steady_clock;
        auto start_tp = Clock::now();
        auto ret =  func(std::forward<Args>(args)...);
        auto duration = duration_cast<milliseconds>(Clock::now() - start_tp);

        return std::make_pair(ret, duration);
    }
}
