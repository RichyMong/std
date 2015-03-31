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

int Epoll::kMaxEpollEvent = 128;

static int kDefaultEpollEvent = EPOLLRDHUP | EPOLLET;

Epoll::Epoll() {
    epfd_ = epoll_create(20);
    if (epfd_ < 0) {
        throw std::runtime_error("cannot create epoll object");
    }
}

Epoll::~Epoll() {
    ::close(epfd_);
}

bool Epoll::add(int event, FileObj *handler) {
    epoll_event info;

    memset(&info, 0, sizeof(info));
    info.events  = convert_event(event) | kDefaultEpollEvent;
    info.data.ptr = handler;

    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, handler->getfd(), &info) < 0) {
        printf("could not add fd[%d] to epoll\n", handler->getfd());
        if (errno != EEXIST) {
            return false;
        }
    }

    return true;
}

bool Epoll::modify(int event, FileObj* handler) {
    epoll_event info;

    memset(&info, 0, sizeof(info));
    info.events  = convert_event(event) | kDefaultEpollEvent;
    info.data.ptr = handler;

    if (epoll_ctl(epfd_, EPOLL_CTL_MOD, handler->getfd(), &info) < 0) {
        printf("could not add fd[%d] to epoll: %s\n",
                handler->getfd(), strerror(errno));
        return false;
    }

    return true;
}

void Epoll::remove(FileObj* file) {
    if (epoll_ctl(epfd_, EPOLL_CTL_DEL, file->getfd(), NULL) < 0) {
        printf("could not remove fd[%d] from epoll\n", file->getfd());
    }

    return;
}

int Epoll::handle_events(int timeout) {
    epoll_event events[kMaxEpollEvent];

    int count = epoll_wait(epfd_, events, kMaxEpollEvent, timeout);
    Time::instance().update();

    if (count < 0) {
        if (errno != EINTR) {
            printf("epoll wait error");
        }
        return 0;
    }

    for (int i = 0; i < count; ++i) {
        FileObj *handler = (FileObj*)(events[i].data.ptr);
        if (events[i].events & EPOLLOUT) {
            handler->on_writeable(*this);
        }
        if (events[i].events & EPOLLIN) {
            handler->on_readable(*this);
        }
    }

    return count > 0 ? count : 0;
}

int Epoll::convert_event(int event) {
    int real_event = 0;

    if (event & ARC_READ_EVENT) {
        real_event = EPOLLIN;
    }

    if (event & ARC_WRITE_EVENT) {
        real_event |= EPOLLOUT;
    }

    if (event & ARC_ONE_SHOT) {
        real_event |= EPOLLONESHOT;
    }

    return real_event;
}

}
