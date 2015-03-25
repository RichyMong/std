#include "epoll.h"
#include "util.h"
#include "time.h"
#include <stdexcept>
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
        throw std::runtime_error("cannot create epoll object");
    }
}

Epoll::~Epoll() {
    close(epfd_);
}

bool Epoll::add(int fd, int event, EventHandler *handler) {
    int real_event = 0;

    if (event & ARC_READ_EVENT) {
        real_event = EPOLLIN;
    }

    if (event & ARC_WRITE_EVENT) {
        real_event |= EPOLLOUT;
    }

    epoll_event info;

    memset(&info, 0, sizeof(info));
    info.events  = real_event | EPOLLRDHUP;
    info.data.ptr = handler;

    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &info) < 0) {
        printf("could not add fd[%d] to epoll\n", fd);
        if (errno != EEXIST) {
            return false;
        }
    }

    return true;
}

bool Epoll::modify(int fd, int event, EventHandler* handler) {
    int real_event = 0;

    if (event & ARC_READ_EVENT) {
        real_event = EPOLLIN;
    }

    if (event & ARC_WRITE_EVENT) {
        real_event |= EPOLLOUT;
    }

    epoll_event info;

    memset(&info, 0, sizeof(info));
    info.events  = event | EPOLLRDHUP;
    info.data.ptr = handler;

    if (epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &info) < 0) {
        printf("could not add fd[%d] to epoll\n", fd);
        return false;
    }

    return true;
}

void Epoll::remove(int fd) {
    if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) < 0) {
        printf("could not remove fd[%d] from epoll\n", fd);
    }

    return;
}

void Epoll::handle_events(int timeout) {
    epoll_event events[kMaxEpollEvent];

    int count = epoll_wait(epfd_, events, kMaxEpollEvent, timeout);
    Time::instance().update();

    if (count < 0) {
        if (errno != EINTR) {
            printf("epoll wait error");
        }
        return;
    }

    for (int i = 0; i < count; ++i) {
        EventHandler *handler = (EventHandler*)(events[i].data.ptr);
        if (events[i].events & EPOLLIN) {
            handler->on_readable(*this);
        } else if (events[i].events & EPOLLOUT) {
            handler->on_writeable(*this);
        }
    }
}

}
