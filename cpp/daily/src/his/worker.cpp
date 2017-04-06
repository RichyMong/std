#include <algorithm>
#include <util/stream.h>
#include "worker.h"

static HeaderWrapper header_wrapper;

void Worker::handle_closed(const net::Client& c)
{
    std::remove(clients_.begin(), clients_.end(), c.shared_from_this());
}

void Worker::handle_packet(const net::Client& c, const char* packet, size_t len)
{
    auto h = reinterpret_cast<const Header*>(packet);
    auto it = send_items_.find(c.id());
    assert(it != send_items_.end());
    auto item = it->second.pop_front();
    assert(item.msg_id = h->msg_id);
    util::InputBufferStream is(reinterpret_cast<const uint8_t*>(packet), len);
}

void Worker::do_jobs()
{
    io_service_.run();
}

void Worker::run()
{
    thread_ = std::thread(&Worker::do_jobs, this);
}

void Worker::wait()
{
    thread_.join();
}
