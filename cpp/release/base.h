#ifndef ROBOT_RELEASE_BASE_H
#define ROBOT_RELEASE_BASE_H

namespace release {

class TCPClient {
public:
    TCPClient() : fd_(-1) { }

    int connect(const char* server, int port);

    int fileno() const { return fd_; }

    void nonblock(bool nblock);

    ssize_t send(const char* buf, size_t len);

    ssize_t recv(const char* buf, size_t size);

private:
    int fd_;
};

}

#endif /* ROBOT_RELEASE_BASE_H */
