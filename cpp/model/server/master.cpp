#include "master.h"
#include "util.h"
#include "timer.h"
#include "signal_event.h"
#include <iostream>
#include <sys/timerfd.h>

namespace server {

Master::~Master() {
    stop_workers();
}

bool Master::start_workers() {
    auto cpu_cnt = util::cpu_cnt();

    std::cout << cpu_cnt << std::endl;
    if (cpu_cnt <= 0) {
        std::cerr << "cannot decide the number of threads to run\n";
        return false;
    }

    for (auto i = 0; i != cpu_cnt; ++i) {
        auto worker = std::make_shared<Worker>(log_);
        workers_.push_back(worker);
    }

    return true;
}

void Master::stop_workers() {
    for (auto i = workers_.begin(); i != workers_.end(); ++i) {
        (*i)->stop();
    }

    workers_.clear();
}

class WorkTimer : public util::TimerObj {
public:
    virtual void timeout() {
        std::cout << "timeouted " << timepoint() << "\n";
    }

    WorkTimer(int msec)
        : util::TimerObj(msec) {
    }
};

int Master::start() {
    // must block the signals before starting any worker thread.
    server::SignalEvent se;

    if (!start_workers()) {
        return 1;
    }

    util::Epoll epoll;
    se.add_event(epoll);
    for ( ; ; ) {
        epoll.handle_events(-1);
    }

    return 0;
}

}
