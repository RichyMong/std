# pragma once

#include <chrono>
#include <type_traits>

namespace util {
    template <typename F, typename... Args>
    using ResultOf = typename std::result_of<F(Args...)>::type;

    using Duration = std::chrono::microseconds;

    template<typename F, typename... Args> inline
    auto timeit(F&& func, Args&&... args) ->
    typename std::enable_if<std::is_void<decltype(func(args...))>::value, Duration>::type
    {
        using namespace std::chrono;
        using Clock = std::chrono::steady_clock;
        auto start_tp = Clock::now();
        func(std::forward<Args>(args)...);
        return duration_cast<Duration>(Clock::now() - start_tp);
    }

    template<typename F, typename... Args> inline
    auto timeit(F&& func, Args&&... args) ->
    typename std::enable_if<!std::is_void<decltype(func(args...))>::value, Duration>::type
    {
        using namespace std::chrono;
        using Clock = std::chrono::steady_clock;
        auto start_tp = Clock::now();
        auto ret =  func(std::forward<Args>(args)...);
        auto duration = duration_cast<Duration>(Clock::now() - start_tp);

        return std::make_pair(ret, duration);
    }
}
