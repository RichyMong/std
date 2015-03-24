#ifndef MODEL_SERVER_WORKER_H
#define MODEL_SERVER_WORKER_H

#include "connection.h"
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
    bool assign_connection(int fd);

    size_t user_cnt() const {
        // we are not guarded by a lock here since it's not important even
        // if we get an imprecise value.
        return users_.size();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<User>> users_;
};

}

#endif // MODEL_SERVER_WORKER_H
