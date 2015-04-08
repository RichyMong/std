#include "master.h"
#include "util.h"
#include "timer.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sys/timerfd.h>

namespace server {

bool worker_cmp(const std::shared_ptr<Worker>& a, const std::shared_ptr<Worker>& b) {
    return a->user_cnt() < b->user_cnt();
}

bool Master::start_workers() {
    auto cpu_cnt = util::cpu_cnt();
    if (cpu_cnt <= 0) {
        throw std::runtime_error("cannot decide the number of threads to run");
    }

    log_->set_log_tag("M0");

    for (auto i = 0; i < cpu_cnt; ++i) {
        auto worker = std::make_shared<Worker>(i + 1, log_);
        workers_.push_back(worker);
    }

    return true;
}

void Master::stop_workers() {
    for (auto& worker : workers_) {
        worker->stop();
    }

    workers_.clear();
}

void Master::add_connection(const ConnPtr& csp) {
    workers_[0]->add_connection(csp);
}

int Master::run() {
    multilex_.add(ARC_READ_EVENT, &sigevent_);

    start_workers();

    for (auto& it : servers_) {
        it.second->set_manager(this);
        multilex_.add(ARC_READ_EVENT | ARC_ONE_SHOT, it.second.get());
    }

    for ( ; ; ) {
        int count = multilex_.handle_events(5000);

        if (count) {
            std::sort(workers_.begin(), workers_.end(), worker_cmp);
        } else {
            for (const auto& worker : workers_) {
                log_->info("worker[%d] user count: %d",
                           worker->worker_id(), worker->user_cnt());
            }
        }
    }

    stop_workers();

    return 0;
}

}
