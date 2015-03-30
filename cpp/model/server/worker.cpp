#include "worker.h"
#include "server.h"
#include "connection.h"
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace server {

class FileDecorator {
public:
    explicit FileDecorator(util::FileObj* fobj)
        : fobj_ { fobj } {
    }
private:
    util::FileObj* fobj_;
};

template <class T>
Worker<T>::Worker(std::shared_ptr<util::Log> log)
    : log_{ log }, running {true} {
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

template <class T>
void Worker<T>::main_loop() {
    multilex_.add(ARC_READ_EVENT, &the_time);

    while (running) {
        multilex_.handle_events(-1);
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

    // Tricky here. We use this method to make us lock free since it's safe
    // to take action on the same epoll fd in muti-thread environment.
    multilex_.add(ARC_WRITE_EVENT | ARC_ONE_SHOT, csp);
}

template <class T>
void Worker<T>::on_readable(util::Multiplex& mplex, util::FileObj* fobj) {
    fobj->on_readable(mplex, fobj);
    if (fobj->is_closed()) {
        std::lock_guard<std::mutex> lck(users_mutex_);
    }
}

template <class T>
void Worker<T>::on_writeable(util::Multiplex&, util::FileObj* fobj) {
    std::shared_ptr<util::Connection> csp(dynamic_cast<util::Connection*>(fobj));
    assert(csp);
    auto usp = std::make_shared<User>(csp, log_);
    users_.insert(std::make_pair(++id_, usp));
    multilex_.modify(ARC_READ_EVENT | ARC_ONE_SHOT, csp.get());
    log_->debug("%lld on_writeable", pthread_self());
    the_time.add_timer(usp);
}

template class Worker<util::Epoll>;

}
