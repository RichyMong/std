#ifndef MODEL_SERVER_EPOLL_H
#define MODEL_SERVER_EPOLL_H

#include <sys/epoll.h>

#define ARC_READ_EVENT  0x01
#define ARC_WRITE_EVENT 0x02

namespace util {

class EventHandler;

class Multiplex {
public:
    virtual ~Multiplex() { }

    virtual bool add(int fd, int event, EventHandler* handler) = 0;

    virtual bool modify(int fd, int event, EventHandler* handler) = 0;

    virtual void remove(int fd) = 0;

    virtual void handle_events(int timeout) = 0;
};

class Epoll : public Multiplex {
public:
    Epoll();

    ~Epoll();

    virtual bool add(int fd, int event, EventHandler* handler);

    virtual bool modify(int fd, int event, EventHandler* handler);

    virtual void remove(int fd);

    virtual void handle_events(int timeout = 0);

private:
    static int kMaxEpollEvent;

private:
    int epfd_;
};

class EventHandler {
public:
    virtual ~EventHandler() { }
    virtual void add_event(Multiplex& mutiplex) = 0;
    virtual void on_readable(Multiplex& mutiplex) = 0;
    virtual void on_writeable(Multiplex& mutiplex) { (void) mutiplex; }
};

}

#endif /* MODEL_SERVER_EPOLL_H */
