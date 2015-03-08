#ifndef ROBOT_RELEASE_BASE_H
#define ROBOT_RELEASE_BASE_H

#include "util.h"
#include "packet.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

namespace release {

class TCPClient {
public:
    TCPClient()
        : fd_(-1), nblock_(true) {
    }

    ~TCPClient();

    bool connect(const std::string& server, int port);

    bool connect(const std::string& addr);

    int sockfd() const { return fd_; }

    void nonblock(bool nblock);

    void closefd();

    ssize_t send(const char* buf, size_t len);

    ssize_t recv(char* buf, size_t len);

    ssize_t peek();

private:
    ssize_t recv(char* buf, size_t len, int flags);

    void set_fd_block();

private:
    int        fd_;

    bool       nblock_;
};

enum class SecureState : int {
    kInit,
    kInProgress,
    kDone,
};

class SSLClient : public TCPClient {
public:
    SSLClient() : security_state_(SecureState::kInit) {
    }

	void generate_key() {
        for (size_t i = 0; i != util::array_size(security_key_); ++i)
            security_key_[0] = rand();
	}

    bool init_security(const std::string& pubkey);

    void complete_security();

    int rsa_encrypt(const std::string& pubkey,
                    unsigned char* data,
                    size_t data_len,
                    unsigned char* buf);

    void send_packet(util::OPacket& pkt);

protected:
    uint32      security_key_[4];
    SecureState security_state_;
};

}

#endif /* ROBOT_RELEASE_BASE_H */
