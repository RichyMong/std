#ifndef MODEL_SERVER_SIGNAL_H
#define MODEL_SERVER_SIGNAL_H

#include "epoll.h"
#include <signal.h>

namespace server {

class SignalEvent : public util::EventHandler {
public:
    SignalEvent();

    ~SignalEvent();

    void on_readable(util::Multiplex& mutiplex);

    void add_event(util::Multiplex& mutiplex) {
        mutiplex.add(sigfd_, ARC_READ_EVENT, this);
    }

private:
    void handle_signal(int signo);

    int      sigfd_;
    sigset_t mask_;
};

}
#endif // MODEL_SERVER_SIGNAL_H
