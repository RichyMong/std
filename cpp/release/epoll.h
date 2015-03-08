#ifndef ROBOT_RELEASE_EPOLL_H
#define ROBOT_RELEASE_EPOLL_H

#include <sys/epoll.h>

namespace util {

class EventHandler;

class Multiplex {
public:
    virtual ~Multiplex() { }

    virtual bool add(int fd, int event, EventHandler* handler) = 0;

    virtual bool modify(int fd, int event, EventHandler* handler) = 0;

    virtual void remove(int fd) = 0;

    virtual void mainloop(int timeout = 0) = 0;
};

class Epoll : public Multiplex {
public:
    Epoll();

    ~Epoll();

    virtual bool add(int fd, int event, EventHandler* handler);

    virtual bool modify(int fd, int event, EventHandler* handler);

    virtual void remove(int fd);

    virtual void mainloop(int timeout = 0);

private:
    static int kMaxEpollEvent;

private:
    int epfd_;
};

class EventHandler {
public:
    virtual ~EventHandler() { }
    virtual void on_readable(Multiplex& mutiplex) = 0;
    virtual void on_writeable(Multiplex& mutiplex) = 0;
    virtual void on_peer_closed(Multiplex& mutiplex) = 0;
};

}

#endif /* ROBOT_RELEASE_EPOLL_H */
