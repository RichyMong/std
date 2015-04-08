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

class ServerManager {
public:
    virtual void add_connection(const ConnPtr& csp) = 0;
};

class Server : public util::FileObj {
public:
    Server(const std::string& addr, short port, const LogPtr& log);

    explicit Server(short port, const LogPtr& log);

    int getfd() const { return listenfd_; }

    void on_readable(util::Multiplex&);

    void set_manager(ServerManager* manager) { manager_ = manager; }

    int port() const { return port_; }

private:
    int  open_listening_socket(const std::string& ipaddr, short port) const;
    void set_options(int fd) const;

    int          listenfd_;
    int          port_;
    LogPtr       log_;
    ServerManager* manager_;
};

}

#endif // MODEL_UTIL_SERVER_H
