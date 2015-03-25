#include "worker.h"
#include <iostream>
#include <sstream>

namespace server {

Worker::~Worker() {
    users_.clear();

    if (thread_.get_id() != std::thread::id {}) {
        thread_.join();
    }
}

void Worker::assign_connection(int fd) {
    users_.insert(std::make_pair(fd, std::unique_ptr<User>(new User(fd))));
}

class WorkTimer : public util::TimerObj {
public:
    virtual void timeout() {
        std::ostringstream oss;
        oss << "worker[" << std::this_thread::get_id() << "]timeouted " << timepoint() << "\n";
        log_->debug(oss.str().c_str());
    }

    WorkTimer(int msec, std::shared_ptr<util::Log> log)
        : util::TimerObj(msec), log_(log) {
    }

private:
    std::shared_ptr<util::Log> log_;
};

void Worker::main_loop() {
    util::Epoll epoll;
    util::Timer timer(1000);
    timer.add_event(epoll);
    timer.add_timer(new WorkTimer(1000, log_));
    while (running) {
        epoll.handle_events(10);
    }
}

void Worker::stop() {
    running = false;
}

}
