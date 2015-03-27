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

class Server : public util::FileEvent {
public:
    Server(const std::string& addr, short port);

    explicit Server(short port);

    bool try_lock_accept();

    void lock_accept();

    void unlock_accept();

    void set_logger(util::LogPtr log) { log_ = log; }

    int getfd() const { return listenfd_; }

    void on_readable(util::Multiplex&);

    void set_manager(ConnManager* manager) { manager_ = manager; }

private:
    static thread_local ConnManager* manager_;
    util::LogPtr log_;
    int          listenfd_;
    std::atomic<std::thread::id> tid_;
};

class ServerManager : public util::Singleston<ServerManager> {
    friend class util::Singleston<ServerManager>;
    typedef std::map<int, std::shared_ptr<Server>> ServerMap;

public:
    void add_server(short port) {
        servers_.insert(std::make_pair(port, std::make_shared<Server>(port)));
    }

    void add_server(short port, const std::shared_ptr<Server>& server) {
        servers_.insert(std::make_pair(port, server));
    }

    void remove_server(short port) {
        servers_.erase(port);
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
