# pragma once

#include <string>

class Config
{
public:
    virtual bool use_remote_server() const = 0;
    virtual bool use_local_server() const = 0;
    virtual std::string app_listen_ip() const = 0;
    virtual int app_listen_port() const = 0;
    virtual std::string backend_listen_ip() const { return "127.0.0.1"; }
    virtual int backend_listen_port() const = 0;
    virtual std::string console_listen_ip() const { return "127.0.0.1"; }
    virtual int console_listen_port() const = 0;
    virtual int get_max_allowed_clients() const = 0;
};


