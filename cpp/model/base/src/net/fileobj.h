# pragma once

namespace net {

class BaseEventLoop;

class FileObj {
public:
    virtual int get_fd() const = 0;
    virtual void handle_readable(const BaseEventLoop& loop) = 0;
    virtual void handle_closed(const BaseEventLoop& loop) = 0;
    virtual void handle_writable(const BaseEventLoop& loop) = 0;
};

}
