#ifndef MODEL_SERVER_WORKER_H
#define MODEL_SERVER_WORKER_H

#include "connection.h"
#include "timer.h"
#include "log.h"
#include "server.h"
#include <string>
#include <thread>
#include <memory>
#include <unordered_map>
#include <mutex>

namespace server {

class UserInfo {
public:
    std::string name_;
    std::string email_;
};

class User : public util::TimerObj {
public:
    User(std::shared_ptr<util::Connection>& sp, util::LogPtr log)
        : TimerObj { 5000 }, log_ { log }, connection_ { sp } {
    }

    void timeout() {
        log_->warning("connection %d timeouted", connection_->getfd());
        connection_->close();
    }

private:
    UserInfo                          userinfo_;
    util::LogPtr                      log_;
    std::shared_ptr<util::Connection> connection_;
};


template <class T>
class Worker : public util::EventHandler {
public:
    explicit Worker(std::shared_ptr<util::Log> log);

    ~Worker();

    void stop();

    void main_loop();

    bool try_assign(const ConnPtr& csp);

    void assign(const ConnPtr& csp);

    size_t user_cnt() const {
        // we are not guarded by a lock here since it's not important even
        // if we get an imprecise value.
        return users_.size();
    }

    void on_readable(util::Multiplex&, util::FileObj*) override;

    void on_writeable(util::Multiplex&, util::FileObj*) override;

    void add_connection(const ConnPtr& csp);

private:
    std::unordered_map<int, std::shared_ptr<User>> users_;
    std::mutex    users_mutex_;
    std::thread   thread_;
    util::LogPtr  log_;
    volatile bool running;
    T             multilex_;
    int           id_ = 0;
};

extern template class Worker<util::Epoll>;

}

#endif // breakMODEL_SERVER_WORKER_H
