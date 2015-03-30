#include "signal_event.h"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/signalfd.h>
#include <assert.h>

namespace server {

SignalEvent::SignalEvent() {
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGHUP);
    sigaddset(&mask, SIGPIPE);

    auto ret = pthread_sigmask(SIG_BLOCK, &mask, NULL);
    if (ret) {
        throw std::runtime_error("cannot block signals");
    }

    sigfd_ = signalfd(-1, &mask, SFD_NONBLOCK);
    if (sigfd_ < 0) {
        throw std::runtime_error("cannot create signal fd");
    }
}

SignalEvent::SignalEvent(const sigset_t& mask) {
    auto ret = pthread_sigmask(SIG_BLOCK, &mask, NULL);
    if (ret) {
        throw std::runtime_error("cannot block signals");
    }

    sigfd_ = signalfd(-1, &mask, SFD_NONBLOCK);
    if (sigfd_ < 0) {
        throw std::runtime_error("cannot create signal fd");
    }
}

SignalEvent::~SignalEvent() {
    ::close(sigfd_);
}

void SignalEvent::on_readable(
    util::Multiplex&, util::FileObj*) {
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
