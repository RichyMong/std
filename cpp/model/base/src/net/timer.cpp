#include "timer.h"

namespace net {

using namespace std;
using namespace std::chrono;

Timer::Timer(TimePoint when, TimerCallBack cb)
    : when_{ when },
      cb_ { cb }
{
}

template <typename T, class Period>
Timer::Timer(duration<T, Period> period, TimerCallBack cb, bool once)
    : period_ { duration_cast<steady_clock::duration>(period) },
      when_ { steady_clock::now() +  period_ },
      cb_ { cb },
      once_ { once }
{
}

void Timer::timeout()
{
    cb_(this);

    if (!once_) {
        when_ = steady_clock::now() + period_;
    }
}

} // namespace net
