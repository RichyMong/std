#include "master.h"
#include "util.h"
#include "timer.h"
#include "signal_event.h"
#include <iostream>
#include <algorithm>
#include <sys/timerfd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace server {

template <class T>
bool worker_cmp(const std::shared_ptr<Worker<T>>& a, const std::shared_ptr<Worker<T>>& b) {
    return a->user_cnt() < b->user_cnt();
}

template <class T>
Master<T>::~Master() {
    stop_workers();
}

template <class T>
bool Master<T>::start_workers() {
    auto cpu_cnt = util::cpu_cnt();

    std::cout << cpu_cnt << std::endl;
    if (cpu_cnt <= 0) {
        std::cerr << "cannot decide the number of threads to run\n";
        return false;
    }

    for (auto i = 0; i != cpu_cnt; ++i) {
        auto worker = std::make_shared<Worker<T>>(log_);
        workers_.push_back(worker);
    }

    return true;
}

template <class T>
void Master<T>::stop_workers() {
    for (auto i = workers_.begin(); i != workers_.end(); ++i) {
        (*i)->stop();
    }

    workers_.clear();
}

template <class T>
int Master<T>::start() {
    // must block the signals before starting any worker thread.
    server::SignalEvent se;

    server_manager.add_server(12345);

    auto servers = server_manager.servers();
    for (auto it = servers.cbegin(); it != servers.cend(); ++it) {
        it->second->set_manager(this);
        it->second->set_logger(log_);
        multilex_.add(ARC_READ_EVENT, it->second.get());
    }

    if (!start_workers()) {
        return 1;
    }

    multilex_.add(ARC_READ_EVENT, &se);

    for ( ; ; ) {
        multilex_.handle_events(-1);
        std::sort(workers_.begin(), workers_.end(), worker_cmp<T>);
        for (auto it = workers_.cbegin(); it != workers_.cend(); ++it) {
            log_->info("cnt: %d\n", (*it)->user_cnt());
        }
    }

    return 0;
}

template <class T>
void Master<T>::add_connection(const ConnPtr& csp) {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    if (getpeername(csp->getfd(), (sockaddr*)&addr, &len) < 0) {
        log_->error("cannot get peer name[fd: %d]\n", csp->getfd());
    } else {
        char buf[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &addr.sin_addr, buf, sizeof(buf));
        log_->info("connect from %s:%hu[fd: %d]\n", buf, ntohs(addr.sin_port), csp->getfd());
    }

    auto it = workers_.begin();
    while (it != workers_.end()) {
        if ((*it++)->try_assign(csp))
            break;
    }

    if (it == workers_.end()) {
        workers_[0]->assign(csp);
    }
}

template class Master<util::Epoll>;

}
