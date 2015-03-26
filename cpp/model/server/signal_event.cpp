#include "signal_event.h"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/signalfd.h>

namespace server {

SignalEvent::SignalEvent() {
    sigemptyset(&mask_);
    sigaddset(&mask_, SIGINT);
    sigaddset(&mask_, SIGHUP);
    sigaddset(&mask_, SIGPIPE);

    auto ret = pthread_sigmask(SIG_BLOCK, &mask_, NULL);
    if (ret) {
        throw std::runtime_error("cannot block signals");
    }

    sigfd_ = signalfd(-1, &mask_, SFD_NONBLOCK);
    if (sigfd_ < 0) {
        throw std::runtime_error("cannot create signal fd");
    }
}

void SignalEvent::on_readable(util::Multiplex& mutiplex) {
    (void) mutiplex;

    for ( ; ; ) {
        signalfd_siginfo fdsi;
        auto nread = read(sigfd_, &fdsi, sizeof(fdsi));
        if (nread == sizeof(fdsi)) {
            handle_signal(fdsi.ssi_signo);
        }

        if (nread < 0) {
            if (errno != EAGAIN && errno != EINTR) {
                perror("read signal");
            }

            break;
        }
    }
}

void SignalEvent::handle_signal(int signo) {
    std::cout << strsignal(signo) << std::endl;
    exit(0);
}

}
