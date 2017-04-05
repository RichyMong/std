# pragma once

#include "header.h"

class Worker : public net::ClientEventHandler
{
public:
    virtual void handle_closed(const net::Client& c) override;

    virtual void handle_packet(const net::Client& c,
                               const char* packet,
                               size_t len) override;

private:
    asio::io_service io_service_;
    std::vector<net::ClientPtr> clients_;
};
