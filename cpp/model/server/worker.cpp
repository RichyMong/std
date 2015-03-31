#include "worker.h"
#include "server.h"
#include "connection.h"
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace server {

class User : public util::TimerObj,
             public util::FileObj,
             public std::enable_shared_from_this<User> {
    template <class T> friend class Worker;
public:
    User(const ConnPtr& sp, util::LogPtr log)
        : TimerObj { 5000 }, log_ { log }, connection_ { sp } {
    }

    std::shared_ptr<User> getptr() {
        return shared_from_this();
    }

    virtual void timeout() override {
        log_->warning("connection %d timeouted", connection_->getfd());
        connection_->close();
        User::user_manager->remove_user(getptr());
    }

    virtual int getfd() const override { return connection_->getfd(); }

    virtual void on_readable(util::Multiplex& mplex) override {
        connection_->on_readable(mplex);

        if (connection_->is_closed()) {
            the_time.del_timer(getptr());
            User::user_manager->remove_user(getptr());
        } else {
            mplex.modify(ARC_READ_EVENT | ARC_ONE_SHOT, this);
        }
    }

    virtual void on_writeable(util::Multiplex& mplex) override {
        User::user_manager->add_user(std::shared_ptr<User>(this));
        mplex.modify(ARC_READ_EVENT | ARC_ONE_SHOT, this);
        log_->debug("%d writeable", getfd());
        the_time.add_timer(getptr());
    }

private:
    // all the user of this thread will by controled by the same manager.
    static thread_local UserManager*  user_manager;

    UserInfo                          userinfo_;
    util::LogPtr                      log_;
    std::shared_ptr<util::Connection> connection_;
};

thread_local UserManager* User::user_manager;

template <class T>
Worker<T>::Worker(int worker_id, std::shared_ptr<util::Log> log)
    : worker_id_ { worker_id }, log_{ log }, running {true} {
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
void Worker<T>::add_user(const UserPtr& uptr) {
    users_.insert(std::make_pair(++id_, uptr));
}

template <class T>
void Worker<T>::remove_user(const UserPtr& uptr) {
    for (auto it = users_.cbegin(); it != users_.cend(); ++it) {
        if (it->second == uptr) {
            users_.erase(it);
            break;
        }
    }
}

template <class T>
void Worker<T>::main_loop() {
    User::user_manager = this;

    std::ostringstream oss;
    oss << 'W' << worker_id_;

    log_->set_thread_tag(oss.str());

    multiplex_.add(ARC_READ_EVENT | ARC_ONE_SHOT, &the_time);

    while (running) {
        multiplex_.handle_events(-1);
    }
}

///////////////////////////////////////////////////////////////////////////////
// The functions below will be called through the main thread.
// Please make sure they are thread-safe.

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
    // we have to use raw pointer here since we don't want the Multiplex
    // module to save a smart pointer. We pass this raw pointer to the
    // worker thread and then it can be taken over by a smart pointer.
    User* user = new User(csp, log_);

    // Tricky here. We use this method to make us lock free since it's safe
    // to take action on the same epoll fd in muti-thread environment.
    multiplex_.add(ARC_WRITE_EVENT | ARC_ONE_SHOT, user);
}

template class Worker<util::Epoll>;

}
