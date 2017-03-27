#include <map>
#include <system_error>
#include <algorithm>
#include <sys/timerfd.h>
#include "timerqueue.h"

namespace net {

using namespace std;
using namespace std::chrono;

class Timer {
public:
    Timer(TimePoint when, TimerCallBack callback)
        : when_{ when },
          callback_ { callback }
    {
    }

    template <typename T, class Period = std::ratio<1> >
    Timer(duration<T, Period> period,
          TimerCallBack callback,
          bool once = false)
        : period_ { duration_cast<steady_clock::duration>(period) },
          when_ { steady_clock::now() +  period_ },
          callback_ { callback },
          once_ { once }
    {
    }

    TimePoint when() const { return when_; }
    bool once() const { return once_; }
    void index(int new_index) { index_ = new_index; }

private:
    steady_clock::duration period_;
    TimePoint when_;
    TimerCallBack callback_;
    bool once_;
    int index_ = -1;
};

class TimerQueueImpl : public FileObj
{
public:
    TimerQueueImpl()
    {
        timerfd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        if (timerfd_ == -1) {
            throw std::system_error(errno, std::generic_category());
        }
    }

    timerid_t add_timer(TimePoint when, TimerCallBack callback)
    {
        if (steady_clock::now() >= when) {
            callback(INVALID_TIMER);
            return INVALID_TIMER;
        }

        timers_.emplace_back(when, callback);
        timers_.back().index(timers_.size() - 1);
        timerid_t id = timers_.back().id();
        up_heap(timers_.size() - 1);
    }

    virtual int get_fd() const override { return timerfd_; }
    virtual void handle_readable(const BaseEventLoop& loop) override;
    virtual void handle_writable(const BaseEventLoop& loop) override;
    virtual void handle_closed(const BaseEventLoop& loop) override;

private:
    void up_heap(size_t index)
    {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (timers_[parent].when() < timers_[index].when()) {
                break;
            }
            swap(timers_[parent], timers_[index]);
        }
    }

    void down_heap(size_t index)
    {
        size_t left = 2 * index + 1;
        while (left < timers_.size()) {
            size_t min_child = left;
            if (left + 1 < timers_.size() &&
                timers_[left + 1].when() < timers_[left].when()) {
                min_child = left + 1;
            }
            if (timers_[index].when() < timers_[min_child].when()) {
                break;
            }
            swap(timers_[index], timers_[min_child]);
            left = index * 2 + 1;
        }
    }

private:
    int timerfd_;
    std::vector<Timer> timers_;
    std::map<timerid_t, size_t> id_indices_;
};

TimerQueue::TimerQueue()
{
    tq_impl_.reset(new TimerQueueImpl);
}

timerid_t TimerQueue::add_timer(TimePoint when, TimerCallBack callback)
{
    return tq_impl_->add_timer(when, callback);
}

void TimerQueue::del_timer(timerid_t id)
{
    tq_impl_->del_timer(id);
}

}
