#ifndef MODEL_SERVER_CONNECTION_H
#define MODEL_SERVER_CONNECTION_H

#include "epoll.h"
#include "log.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>

namespace util {

class Connection : public util::FileObj {
public:
    Connection(int fd, util::LogPtr log)
        : sockfd_ { fd }, log_ { log } {
        assert(sockfd_ != -1);
    }

    virtual ~Connection() = default;

    void close() {
        ::shutdown(sockfd_, SHUT_RDWR);
        ::close(sockfd_);
        sockfd_ = -1;
    }

    int getfd() const { return sockfd_; }

    void setblockstate(bool blocked);

    bool is_closed() const { return sockfd_ == -1; }

    void on_readable(util::Multiplex&) {
        char buf[4096];
        auto nread = read(sockfd_, buf, sizeof(buf));
        if (nread > 0) {
            log_->debug("%s", buf);
        } else {
            close();
        }
    }

private:
    Connection(const Connection&);
    Connection& operator=(const Connection&);

    int          sockfd_;
    util::LogPtr log_;
};

}

// typedef std::shared_ptr<util::Connection> ConnPtr;
typedef util::Connection* ConnPtr;

#endif // MODEL_SERVER_CONNECTION_H
