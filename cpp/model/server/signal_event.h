#ifndef MODEL_SERVER_SIGNAL_H
#define MODEL_SERVER_SIGNAL_H

#include "epoll.h"
#include <signal.h>

namespace server {

class SignalEvent : public util::FileObj {
public:
    SignalEvent();

    explicit SignalEvent(const sigset_t& mask);

    ~SignalEvent();

    int getfd() const { return sigfd_; }

    void on_readable(util::Multiplex& mutiplex, util::FileObj* handler);

private:
    void handle_signal(int signo);

    int      sigfd_;
};

}
#endif // MODEL_SERVER_SIGNAL_H
