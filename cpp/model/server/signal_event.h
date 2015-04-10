#ifndef MODEL_SERVER_SIGNAL_H
#define MODEL_SERVER_SIGNAL_H

#include "epoll.h"
#include <initializer_list>
#include <signal.h>

namespace server {

class SignalEvent : public util::FileObj {
public:
    SignalEvent();

    explicit SignalEvent(const std::initializer_list<int>& signals);

    ~SignalEvent();

    int getfd() const { return sigfd_; }

    void on_readable(util::Multiplex& mutiplex);

private:
    void handle_signal(int signo);

    int      sigfd_;
};

}
#endif // MODEL_SERVER_SIGNAL_H
