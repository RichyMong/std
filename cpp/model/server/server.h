#ifndef MODEL_UTIL_SERVER_H
#define MODEL_UTIL_SERVER_H

#include "epoll.h"
#include "util.h"
#include "log.h"
#include "connection.h"
#include <string>
#include <map>
#include <atomic>

namespace server {

class ConnManager {
public:
    virtual void add_connection(const ConnPtr& csp) = 0;
};

class Server : public util::FileObj {
public:
    Server(const std::string& addr, short port);

    explicit Server(short port);

    void set_logger(util::LogPtr log) { log_ = log; }

    int getfd() const { return listenfd_; }

    void on_readable(util::Multiplex&);

    void set_manager(ConnManager* manager) { manager_ = manager; }

    int port() const { return port_; }

private:
    ConnManager* manager_;
    util::LogPtr log_;
    int          listenfd_;
    int          port_;
};

class ServerManager : public util::Singleston<ServerManager> {
    friend class util::Singleston<ServerManager>;
    typedef std::map<int, std::shared_ptr<Server>> ServerMap;

public:
    void add_server(const std::shared_ptr<Server>& server) {
        servers_.insert(std::make_pair(server->port(), server));
    }

    void remove_server(int port) {
        servers_.erase(port);
    }

    void remove_server(const std::shared_ptr<Server>& server) {
        remove_server(server->port());
    }

    ServerMap& servers() {
        return servers_;
    }

private:
    ServerManager() { }
    ServerManager(const ServerManager&);

    ServerMap servers_;
};

}

#define server_manager server::ServerManager::instance()

#endif // MODE_UTIL_SERVER_H
