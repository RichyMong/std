#ifndef MODEL_SERVER_EPOLL_H
#define MODEL_SERVER_EPOLL_H

#include <unistd.h>
#include <sys/epoll.h>

#define ARC_READ_EVENT  0x01
#define ARC_WRITE_EVENT 0x02

namespace util {

class FileEvent;

class Multiplex {
public:
    virtual ~Multiplex() { }

    virtual bool add(int event, FileEvent* handler) = 0;

    virtual bool modify(int event, FileEvent* handler) = 0;

    virtual void remove(FileEvent* handler) = 0;

    virtual void handle_events(int timeout) = 0;
};

class Epoll : public Multiplex {
public:
    Epoll();

    ~Epoll();

    virtual bool add(int event, FileEvent* handler);

    virtual bool modify(int event, FileEvent* handler);

    virtual void remove(FileEvent* file);

    virtual void handle_events(int timeout = 0);

private:
    static int kMaxEpollEvent;

private:
    int epfd_;
};

class FileEvent {
public:
    virtual ~FileEvent() { }

    virtual int getfd() const = 0;

    virtual void on_readable(Multiplex& mutiplex) = 0;

    virtual void on_writeable(Multiplex&) { }
};

}

#endif /* MODEL_SERVER_EPOLL_H */
