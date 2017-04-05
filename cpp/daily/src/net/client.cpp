#include "client.h"

std::atomic<int> net::Client::next_client_id { 1 };

namespace net {

Client::Client(asio::io_service& service, BaseHeader* header)
    : socket_(service),
      id_(next_client_id++),
      header_(header)
{
}

void Client::run()
{
    read_header();
}

void Client::read_header()
{
    recv_buffer_.resize(header_->size());
    asio::async_read(socket_,
        asio::buffer(&*recv_buffer_.begin(), recv_buffer_.size()),
        std::bind(&Client::handle_read_header,
            this->shared_from_this(), asio::placeholders::error
        )
    );
}

void Client::handle_read_header(const std::error_code& error)
{
    recv_buffer_.resize(header_->pkg_size(recv_buffer_));
    if (!error) {
        asio::async_read(socket_,
            asio::buffer(&*recv_buffer_.begin(), recv_buffer_.size()),
            std::bind(&Client::handle_read_body,
                this->shared_from_this(), asio::placeholders::error
            )
        );
    } else {
        handler_->handle_closed(*this);
        socket_.close();
    }
}

void Client::handle_read_body(const std::error_code& error)
{
    if (!error) {
        assert(header_->is_complete(recv_buffer_));
        handler_->handle_packet(*this, recv_buffer_);
    } else {
        handler_->handle_closed(*this);
        socket_.close();
    }
}

} // namespace net
