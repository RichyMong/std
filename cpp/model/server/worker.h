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

class User;

typedef std::shared_ptr<User> UserPtr;

class UserManager {
public:
    virtual ~UserManager() { }

    virtual void add_user(const UserPtr& csp) = 0;

    virtual void remove_user(const UserPtr& csp) = 0;
};

class Worker : public UserManager,
               public std::enable_shared_from_this<Worker> {
public:
    Worker(int worker_id, const LogPtr& log);

    ~Worker();

    Worker(const Worker&) = delete;

    Worker& operator=(const Worker&) = delete;

    void stop();

    void main_loop();

    bool try_assign(const ConnPtr& csp);

    void assign(const ConnPtr& csp);

    int worker_id() const { return worker_id_; }

    size_t user_cnt() const {
        // we are not guarded by a lock here since it's not important even
        // if we get an imprecise value.
        return users_.size();
    }

    void add_connection(const ConnPtr& csp);

    virtual void add_user(const UserPtr& csp) override;

    virtual void remove_user(const UserPtr& csp) override;

private:
    const int     worker_id_;
    std::unordered_map<int, std::shared_ptr<User>> users_;
    std::mutex    users_mutex_;
    std::thread   thread_;
    LogPtr        log_;
    volatile bool running_;
    util::Epoll   multiplex_;
    int           id_ = 0;
};

}

#endif // breakMODEL_SERVER_WORKER_H
