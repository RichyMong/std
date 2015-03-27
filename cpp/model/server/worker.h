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

class User : public util::Connection {
public:
    User(int fd, util::LogPtr log)
        : util::Connection(fd, log) {
    }

private:
    UserInfo userinfo_;
};


template <class T>
class Worker : public ConnManager {
public:
    explicit Worker(std::shared_ptr<util::Log> log);

    ~Worker();

    void stop();

    void main_loop();

    bool try_assign(const ConnPtr& csp);

    void assign(const ConnPtr& csp);

    void add_connection(const ConnPtr& csp);

    size_t user_cnt() const {
        // we are not guarded by a lock here since it's not important even
        // if we get an imprecise value.
        return users_.size();
    }

private:
    std::unordered_map<int, ConnPtr> users_;
    std::mutex    users_mutex_;
    util::LogPtr  log_;
    util::Timer   timer_;
    std::thread   thread_;
    volatile bool running;
    T             multilex_;
    int           id_ = 0;
};

extern template class Worker<util::Epoll>;

}

#endif // MODEL_SERVER_WORKER_H
