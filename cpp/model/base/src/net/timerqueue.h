# pragma once

#include <vector>
#include "fileobj.h"
#include "timer.h"

namespace net {

class TimerQueue : public FileObj {
public:
    TimerQueue();

    bool add_timer(Timer* timer);
    void del_timer(Timer* timer);

    virtual int get_fd() const override { return timerfd_; }
    virtual void handle_readable(const BaseEventLoop& loop) override;
    virtual void handle_writable(const BaseEventLoop& loop) override;
    virtual void handle_closed(const BaseEventLoop& loop) override;

private:
    struct TimerComparator {
        bool operator()(const Timer& lhs, const Timer& rhs) const {
            return lhs.when() < rhs.when();
        }
    };

    int timerfd_;
    std::vector<Timer*> timers_;
};

}
