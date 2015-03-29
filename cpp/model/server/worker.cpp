#include "worker.h"
#include "server.h"
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace server {

template <class T>
Worker<T>::Worker(std::shared_ptr<util::Log> log)
    : log_{ log }, timer_ { 1000 }, running {true} {
    thread_ = std::thread { &Worker::main_loop, this };
    if (thread_.get_id() == std::thread::id{}) {
        throw std::runtime_error("cannot start worker thread!");
    }
}

template <class T>
Worker<T>::~Worker() {
    users_.clear();

    thread_.join();
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

template <class T>
void Worker<T>::main_loop() {
    timer_.add_timer(new WorkTimer(1000, log_));
    multilex_.add(ARC_READ_EVENT, &timer_);
    while (running) {
        multilex_.handle_events(50);
    }
}

template <class T>
bool Worker<T>::try_assign(const ConnPtr& csp) {
    std::unique_lock<std::mutex> lck { users_mutex_, std::defer_lock };
    if (lck.try_lock()) {
        add_connection(csp);
        return true;
    }
    return false;
}

template <class T>
void Worker<T>::assign(const ConnPtr& csp) {
    std::lock_guard<std::mutex> lck(users_mutex_);
    add_connection(csp);
}

template <class T>
void Worker<T>::stop() {
    running = false;
}

template <class T>
void Worker<T>::add_connection(const ConnPtr& csp) {
    csp->set_handler(this);
    multilex_.add(ARC_READ_EVENT, csp.get());
    users_.insert(std::make_pair(++id_, csp));
}

template class Worker<util::Epoll>;

}
