#include "epoll.h"
#include "util.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <cstdio>
#include <errno.h>

namespace util {

int Epoll::kMaxEpollEvent = 20;

Epoll::Epoll() {
    epfd_ = epoll_create(20);
    if (epfd_ < 0) {
        err_exit(true, "cannot create epoll fd\n");
    }
}

Epoll::~Epoll() {
    close(epfd_);
}

bool Epoll::add(int fd, int event, EventHandler *handler) {
    epoll_event info;

    memset(&info, 0, sizeof(info));
    info.events  = event | EPOLLRDHUP;
    info.data.ptr = handler;

    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &info) < 0) {
        err_print("could not add fd[%d] to epoll\n", fd);
        if (errno != EEXIST) {
            return false;
        }
    }

    return true;
}

bool Epoll::modify(int fd, int event, EventHandler* handler) {
    epoll_event info;

    memset(&info, 0, sizeof(info));
    info.events  = event | EPOLLRDHUP;
    info.data.fd = fd;
    info.data.ptr = handler;

    if (epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &info) < 0) {
        err_print("could not add fd[%d] to epoll\n", fd);
        return false;
    }

    return true;
}

void Epoll::remove(int fd) {
    if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) < 0) {
        err_print("could not remove fd[%d] from epoll\n", fd);
    }

    return;
}

void Epoll::mainloop(int timeout) {
    for ( ; ; ) {
        epoll_event events[kMaxEpollEvent];

        int count = epoll_wait(epfd_, events, kMaxEpollEvent, timeout);
        for (int i = 0; i != count; ++i) {
            EventHandler *handler = (EventHandler*)(events[i].data.ptr);
            if (events[i].events & EPOLLOUT) {
                handler->on_writeable(*this);
            } else if (events[i].events & EPOLLIN) {
                handler->on_readable(*this);
            } else if (events[i].events & EPOLLRDHUP) {
                handler->on_peer_closed(*this);
            }
        }

        if (count < 0 && errno != EINTR) {
            err_print("epoll wait error");
        }
    }
}

}
