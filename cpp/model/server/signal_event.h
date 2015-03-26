#ifndef MODEL_SERVER_SIGNAL_H
#define MODEL_SERVER_SIGNAL_H

#include "epoll.h"
#include <signal.h>

namespace server {

class SignalEvent : public util::FileEvent {
public:
    SignalEvent();

    int getfd() const { return sigfd_; }

    void on_readable(util::Multiplex& mutiplex);

private:
    void handle_signal(int signo);

    int      sigfd_;

    sigset_t mask_;
};

}
#endif // MODEL_SERVER_SIGNAL_H
