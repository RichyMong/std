#ifndef MODEL_SERVER_CONNECTION_H
#define MODEL_SERVER_CONNECTION_H

#include <string>
#include <unistd.h>

namespace util {

class Connection {
public:
    explicit Connection(int fd);

    void close() {
        ::close(sockfd_);
    }

    void setblockstate(bool blocked);

    bool is_nonblocking() const;

private:
    Connection(const Connection&);
    Connection& operator=(const Connection&);

    int         sockfd_;
    std::string peerinfo_;
};

}

#endif // MODEL_SERVER_CONNECTION_H
