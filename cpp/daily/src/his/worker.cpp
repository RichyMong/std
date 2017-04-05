#include <algorithm>
#include "worker.h"

static HeaderWrapper header_wrapper;

void Worker::handle_closed(const net::Client& c)
{
    std::remove(clients_.begin(), clients_.end(), c.shared_from_this());
}

void Worker::handle_packet(const net::Client& c, const char* packet, size_t len)
{

}
