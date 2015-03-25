#ifndef MODEL_SERVER_MASTER_H
#define MODEL_SERVER_MASTER_H

#include "log.h"
#include "worker.h"
#include <vector>
#include <memory>

namespace server {

class Master {
public:
    Master(std::shared_ptr<util::Log> log)
        : log_ { log } {
    }

    ~Master();

    int start();

private:
    bool start_workers();

    void stop_workers();

    typedef std::shared_ptr<server::Worker> WorkerPtr;

    std::shared_ptr<util::Log> log_;
    std::vector<WorkerPtr>     workers_;
};

}

#endif // MODEL_SERVER_MASTER_H
