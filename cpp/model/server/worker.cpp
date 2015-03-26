#include "worker.h"
#include "server.h"
#include <iostream>
#include <sstream>

namespace server {

template <typename T>
Worker<T>::~Worker() {
    users_.clear();

    if (thread_.get_id() != std::thread::id {}) {
        thread_.join();
    }
}

class WorkTimer : public util::TimerObj {
public:
    virtual void timeout() {
    }

    WorkTimer(int msec, std::shared_ptr<util::Log> log)
        : util::TimerObj(msec), log_(log) {
    }

private:
    std::shared_ptr<util::Log> log_;
};

template <typename T>
void Worker<T>::main_loop() {
    util::Timer timer(1000);
    timer.add_timer(new WorkTimer(1000, log_));
    multilex_.add(ARC_READ_EVENT, &timer);
    while (running) {
        multilex_.handle_events(10);
    }
}

template <typename T>
void Worker<T>::assign(const ConnPtr& csp) {
    std::lock_guard<std::mutex> lck(users_mutex_);
    users_.insert(std::make_pair(++id_, csp));
    multilex_.add(ARC_READ_EVENT, csp.get());
    log_->debug("add fd[%d]\n", csp.get()->getfd());
}

template <typename T>
void Worker<T>::stop() {
    running = false;
}

template class Worker<util::Epoll>;

}
