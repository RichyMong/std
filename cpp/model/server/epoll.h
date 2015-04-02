#ifndef MODEL_SERVER_EPOLL_H
#define MODEL_SERVER_EPOLL_H

#define ARC_READ_EVENT  0x01
#define ARC_WRITE_EVENT 0x02
#define ARC_ONE_SHOT    0x04

namespace util {

class FileObj;

class Multiplex {
public:
    virtual ~Multiplex() = default;

    virtual bool add(int event, FileObj* handler) = 0;

    virtual bool modify(int event, FileObj* handler) = 0;

    virtual void remove(FileObj* handler) = 0;

    virtual int handle_events(int timeout) = 0;
};

class Epoll : public Multiplex {
public:
    Epoll();

    ~Epoll();

    virtual bool add(int event, FileObj* handler);

    virtual bool modify(int event, FileObj* handler);

    virtual void remove(FileObj* file);

    virtual int handle_events(int timeout = 0);

    static int convert_event(int event);

private:
    static int kMaxEpollEvent;

private:
    int epfd_;
};

class FileObj {
public:
    virtual ~FileObj() = default;

    virtual int getfd() const = 0;

    bool is_closed() const { return getfd() == -1; }

    virtual void on_readable(Multiplex&) = 0;

    virtual void on_writeable(Multiplex&) { }
};

}

#endif /* MODEL_SERVER_EPOLL_H */
