#include "master.h"
#include "util.h"
#include "timer.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sys/timerfd.h>

namespace server {

template <class T>
bool worker_cmp(const std::shared_ptr<Worker<T>>& a, const std::shared_ptr<Worker<T>>& b) {
    return a->user_cnt() < b->user_cnt();
}

template <class T>
Master<T>::~Master() {
}

template <class T>
bool Master<T>::start_workers() {
    auto cpu_cnt = util::cpu_cnt();

    std::cout << cpu_cnt << std::endl;
    if (cpu_cnt <= 0) {
        throw std::runtime_error("cannot decide the number of threads to run");
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
void Master<T>::add_connection(const ConnPtr& csp) {
    workers_[0]->add_connection(csp);
    return;
    for (auto it = workers_.begin(); it != workers_.end(); ++it) {
        // if ((*it)->try_assign(csp)) {
        //     return;
        // }
    }

    // hope we won't be blocked for a long time
    // workers_[0]->assign(csp);
}

template <class T>
int Master<T>::start() {
    server_manager.add_server(12345);

    start_workers();

    auto servers = server_manager.servers();
    for (auto it = servers.cbegin(); it != servers.cend(); ++it) {
        it->second->set_manager(this);
        it->second->set_logger(log_);
        multilex_.add(ARC_READ_EVENT | ARC_ONE_SHOT, it->second.get());
    }

    multilex_.add(ARC_READ_EVENT, &sigevent_);

    for ( ; ; ) {
        int count = multilex_.handle_events(5000);

        if (count) {
            std::sort(workers_.begin(), workers_.end(), worker_cmp<T>);
            continue;
        }

        for (auto it = workers_.cbegin(); it != workers_.cend(); ++it) {
            log_->info("cnt: %d", (*it)->user_cnt());
        }
    }

    stop_workers();

    return 0;
}

template class Master<util::Epoll>;

}
