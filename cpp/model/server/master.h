#ifndef MODEL_SERVER_MASTER_H
#define MODEL_SERVER_MASTER_H

#include "worker.h"
#include <vector>
#include <memory>

namespace server {

class Master {
public:
    void run();

private:
    typedef std::shared_ptr<server::Worker> WorkerPtr;

    std::vector<WorkerPtr> workers_;
};

}

#endif // MODEL_SERVER_MASTER_H
