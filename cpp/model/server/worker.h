#ifndef MODEL_SERVER_WORKER_H
#define MODEL_SERVER_WORKER_H

#include "connection.h"
#include "timer.h"
#include "log.h"
#include <string>
#include <thread>
#include <memory>
#include <unordered_map>

namespace server {

class UserInfo {
public:
    std::string name_;
    std::string email_;
};

class User : public util::Connection {
public:
    explicit User(int sockfd)
        : util::Connection(sockfd) {
    }

private:
    UserInfo userinfo_;
};


class Worker {
public:
    explicit Worker(std::shared_ptr<util::Log> log)
        : log_{ log }, timer_ { 2000 }, running {true} {
        thread_ = std::thread { &Worker::main_loop, this };
    }

    ~Worker();

    void stop();

    void assign_connection(int fd);

    void main_loop();

    size_t user_cnt() const {
        // we are not guarded by a lock here since it's not important even
        // if we get an imprecise value.
        return users_.size();
    }

private:
    std::unordered_map<int, std::unique_ptr<User>> users_;
    std::shared_ptr<util::Log> log_;
    util::Timer timer_;
    std::thread thread_;
    volatile bool running;
};

}

#endif // MODEL_SERVER_WORKER_H
