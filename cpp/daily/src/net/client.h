#include <memory>
#include <atomic>
#include <cstdint>
#include <asio.hpp>

namespace net {

class BaseHeader
{
public:
    virtual ~BaseHeader() {  }

    // The size of this header
    virtual size_t size() const = 0;

    // The size of the packet which contains this header. The buffer
    // must be at least size() length.
    virtual size_t pkg_size(const char* buf, size_t len) const = 0;

    // If the given buffer contains at least one complete packet.
    virtual bool is_complete(const char* pkg, size_t len) const = 0;

    virtual size_t pkg_size(const std::vector<char>& buffer)
    {
        return pkg_size(&*buffer.begin(), buffer.size());
    }

    virtual bool is_complete(const std::vector<char>& buffer)
    {
        return is_complete(&*buffer.begin(), buffer.size());
    }
};

class Client;
class ClientEventHandler
{
public:
    virtual ~ClientEventHandler() {  }

    virtual void handle_closed(const Client& c) = 0;

    virtual void handle_packet(const Client& c, const char* pkg, size_t len) = 0;

    virtual void handle_packet(const Client& c, const std::vector<char>& buffer)
    {
        handle_packet(c, &*buffer.begin(), buffer.size());
    }
};

class Client : public std::enable_shared_from_this<Client>
{
public:
    Client(asio::io_service& service, BaseHeader* header);

    void run();

    void set_handler(ClientEventHandler* handler) { handler_ = handler; }

    asio::ip::tcp::socket& socket() { return socket_; }

    uint64_t id() const { return id_; }

private:
    void read_header();
    void handle_read_header(const std::error_code& error);
    void handle_read_body(const std::error_code& error);

    static std::atomic<int> next_client_id;

    asio::ip::tcp::socket socket_;
    const uint64_t id_;
    BaseHeader* header_;
    ClientEventHandler* handler_;
    std::vector<char> recv_buffer_;
};

typedef std::shared_ptr<Client> ClientPtr;

}

