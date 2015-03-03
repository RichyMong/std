#include "base.h"
#include "util.h"
#include <sstream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace release {

int TCPClient::connect(const char* server, int port) {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0) {
        util::err_exit(true, "Failed to connect to %s:%d\n", server, port);
    }

    std::ostringstream sport;
    sport << port;

    addrinfo *res, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int err_code = getaddrinfo(server, sport.str().c_str(), &hints, &res);
    if (err_code < 0) {
        ::close(fd_);
        util::err_exit(false, "getaddrinfo error: %s\n", gai_strerror(err_code));
    }
}

void TCPClient::nonblock(bool nblock) {
    int flag = fcntl(fd_, F_GETFL);
    if (flag < 0) {
        fprintf(stderr, "could not get file[%d] flag: %s\n", fd_, strerror(errno));
        return;
    }

    if (nblock) {
        flag |= O_NONBLOCK;
    } else {
        flag &= ~O_NONBLOCK;
    }

    if (fntl(fd_, F_SETFL, flag) < 0) {
        fprintf(stderr, "could not set file[%d] flag: %s\n", fd_, strerror(errno));
    }
}

}
