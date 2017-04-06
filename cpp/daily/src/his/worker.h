# pragma once

#include <map>
#include <deque>
#include <thread>
#include "header.h"

static constexpr size_t STOCK_ID_LEN = 16;

enum class MessageType : uint8_t
{
    MARKET,
    STOCK,
};

union MetaInfo {
    char stock[STOCK_ID_LEN];
    uint16_t market;
};

struct MessageItem
{
    MetaInfo meta;
    uint16_t id;
    MessageType  type;
};

class Worker : public net::ClientEventHandler
{
public:
    void run();

    void wait();

    virtual void handle_closed(const net::Client& c) override;

    virtual void handle_packet(const net::Client& c,
                               const char* packet,
                               size_t len) override;

private:
    void do_jobs();

    asio::io_service io_service_;
    std::vector<net::ClientPtr> clients_;
    std::map<int, std::deque<MessageItem> > send_items_;
    std::thread thread_;
};
