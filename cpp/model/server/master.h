#ifndef MODEL_SERVER_MASTER_H
#define MODEL_SERVER_MASTER_H

#include "log.h"
#include "worker.h"
#include <vector>
#include <memory>

namespace server {

template <class MulPlex>
class Master : public ConnManager {
public:
    Master(std::shared_ptr<util::Log> log)
        : log_ { log } {
    }

    ~Master();

    int start();

    void add_connection(const ConnPtr& csp);

private:
    bool start_workers();

    void stop_workers();

    typedef std::shared_ptr<Worker<MulPlex>> WorkerPtr;

    std::shared_ptr<util::Log> log_;
    std::vector<WorkerPtr>     workers_;
    MulPlex                    multilex_;
};

extern template class Master<util::Epoll>;

}

#endif // MODEL_SERVER_MASTER_H
