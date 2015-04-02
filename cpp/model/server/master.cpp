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
bool Master<T>::start_workers() {
    auto cpu_cnt = util::cpu_cnt();
    if (cpu_cnt <= 0) {
        throw std::runtime_error("cannot decide the number of threads to run");
    }

    log_->set_log_tag("M0");

    for (auto i = 0; i < cpu_cnt; ++i) {
        auto worker = std::make_shared<Worker<T>>(i + 1, log_);
        workers_.push_back(worker);
    }

    return true;
}

template <class T>
void Master<T>::stop_workers() {
    for (auto& worker : workers_) {
        worker->stop();
    }

    workers_.clear();
}

template <class T>
void Master<T>::add_connection(const ConnPtr& csp) {
    workers_[0]->add_connection(csp);
}

template <class T>
int Master<T>::run() {
    multilex_.add(ARC_READ_EVENT, &sigevent_);

    start_workers();

    server_manager.add_server(std::make_shared<Server>(12345));

    auto servers = server_manager.servers();
    for (auto& it : servers) {
        it.second->set_manager(this);
        it.second->set_logger(log_);
        multilex_.add(ARC_READ_EVENT | ARC_ONE_SHOT, it.second.get());
    }

    for ( ; ; ) {
        int count = multilex_.handle_events(5000);

        if (count) {
            std::sort(workers_.begin(), workers_.end(), worker_cmp<T>);
        } else {
            for (const auto& worker : workers_) {
                log_->info("cnt: %d", worker->user_cnt());
            }
        }
    }

    stop_workers();

    return 0;
}

template class Master<util::Epoll>;

}
