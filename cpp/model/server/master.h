#ifndef MODEL_SERVER_MASTER_H
#define MODEL_SERVER_MASTER_H

#include "log.h"
#include "worker.h"
#include "signal_event.h"
#include <vector>
#include <memory>

namespace server {

class Master : public ServerManager {
    typedef std::shared_ptr<Worker> WorkerPtr;
    typedef std::map<int, std::shared_ptr<Server>> ServerMap;
public:
    explicit Master(const LogPtr& log)
        : log_ { log } {
    }

    virtual ~Master() = default;

    Master(const Master&) = delete;
    Master& operator=(const Master&) = delete;

    void add_connection(const ConnPtr& csp);

    int run();

    void add_server(const std::shared_ptr<Server>& server) {
        servers_.insert(std::make_pair(server->port(), server));
    }

    void remove_server(int port) {
        servers_.erase(port);
    }

    void remove_server(const std::shared_ptr<Server>& server) {
        remove_server(server->port());
    }

private:
    bool start_workers();

    void stop_workers();

    util::Epoll            multilex_;
    LogPtr                 log_;
    ServerMap              servers_;
    std::vector<WorkerPtr> workers_;
    server::SignalEvent    sigevent_;
};

}

#endif // MODEL_SERVER_MASTER_H
