#ifndef MODEL_SERVER_MASTER_H
#define MODEL_SERVER_MASTER_H

#include "log.h"
#include "worker.h"
#include "signal_event.h"
#include <vector>
#include <memory>

namespace server {

template <class MulPlex>
class Master : public ConnManager {
    typedef std::shared_ptr<Worker<MulPlex>> WorkerPtr;
public:
    explicit Master(const LogPtr& log)
        : log_ { log } {
    }

    virtual ~Master() = default;

    Master(const Master&) = delete;
    Master& operator=(const Master&) = delete;

    void add_connection(const ConnPtr& csp);

    int run();

private:
    bool start_workers();

    void stop_workers();

    MulPlex                multilex_;
    LogPtr                 log_;
    std::vector<WorkerPtr> workers_;
    server::SignalEvent    sigevent_;
};

extern template class Master<util::Epoll>;

}

#endif // MODEL_SERVER_MASTER_H
