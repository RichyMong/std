#include "server.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>

namespace server {

Server::Server(const std::string& ipaddr, short port, const LogPtr& log)
    : port_ { port }, log_ { log }, manager_ { NULL } {
    listenfd_ = open_listening_socket(ipaddr, port);
    if (listenfd_ < 0) {
        throw std::runtime_error("cannot listen port");
    }
}

Server::Server(short port, const LogPtr& log)
    : Server("0.0.0.0", port, log) {
}

void Server::set_options(int fd) const {
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        log_->error("failed set SO_REUSEADDR option");
    }

    int nb = 1;
    if (ioctl(fd, FIONBIO, &nb) < 0) {
        log_->error("failed to set non-block");
    }
}

int Server::open_listening_socket(const std::string& ipaddr, short port) const {
    std::ostringstream sport;
    sport << port;

    addrinfo *result, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_flags    = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;

    int err_code = getaddrinfo(ipaddr.c_str(), sport.str().c_str(), &hints, &result);
    if (err_code < 0) {
        return -1;
    }

    int fd = -1;

    /* getaddrinfo() returns a list of address structures.
      Try each address until we successfully connect(2).
      If socket(2) (or connect(2)) fails, we (close the socket
      and) try the next address. */
    for (addrinfo* rp = result; rp != NULL; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == -1)
           continue;

        if (::bind(fd, rp->ai_addr, rp->ai_addrlen) == 0 &&
            ::listen(fd, 511) == 0) {
            set_options(fd);
            break;
        }

        ::close(fd);
        fd = -1;
    }

    freeaddrinfo(result);

    return fd;
}

void Server::on_readable(util::Multiplex& multilex_) {
    for ( ; ; ) {
        auto fd = accept(listenfd_, NULL, NULL);
        if (fd >= 0 && manager_) {
            manager_->add_connection(ConnPtr(new util::Connection(fd, log_)));
        }

        if (errno != EINTR)
            break;
    }

    multilex_.modify(ARC_READ_EVENT | ARC_ONE_SHOT, this);
}

}
