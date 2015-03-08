#include "base.h"
#include "packet.h"
#include "proto.h"
#include <sstream>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

namespace release {

TCPClient::~TCPClient() {
    if (fd_ >= 0) {
        closefd();
    }
}

void TCPClient::closefd() {
    assert(fd_ >= 0);
    ::close(fd_);
    fd_ = -1;
}

bool TCPClient::connect(const std::string& server, int port) {
    std::ostringstream sport;
    sport << port;

    addrinfo *result, hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int err_code = getaddrinfo(server.c_str(), sport.str().c_str(), &hints, &result);
    if (err_code < 0) {
        ::close(fd_);
        util::err_exit(false, "getaddrinfo error: %s\n", gai_strerror(err_code));
    }

    /* getaddrinfo() returns a list of address structures.
      Try each address until we successfully connect(2).
      If socket(2) (or connect(2)) fails, we (close the socket
      and) try the next address. */
    for (addrinfo* rp = result; rp != NULL; rp = rp->ai_next) {
        fd_ = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd_ == -1)
           continue;

        if (nblock_) {
            set_fd_block();
        }

        if (::connect(fd_, rp->ai_addr, rp->ai_addrlen) != -1 ||
            errno == EINPROGRESS) {
            break;
        }

        close(fd_);
        fd_ = -1;
    }

    freeaddrinfo(result);           /* No longer needed */

    if (fd_ < 0) {               /* No address succeeded */
       util::err_print("Could not connect to %s:%d", server.c_str(), port);
       return false;
    }

    return true;
}

bool TCPClient::connect(const std::string& addr) {
    auto pos = addr.find(':');
    assert(pos != std::string::npos);
    return connect(addr.substr(0, pos), atoi(addr.substr(pos + 1).c_str()));
}

void TCPClient::nonblock(bool nblock) {
    nblock_ = nblock;

    if (fd_ >= 0) {
        set_fd_block();
    }
}

void TCPClient::set_fd_block() {
    int flag = fcntl(fd_, F_GETFL);
    if (flag < 0) {
        util::err_print("could not get fd[%d] flag", fd_);
        return;
    }

    if (nblock_) {
        flag |= O_NONBLOCK;
    } else {
        flag &= ~O_NONBLOCK;
    }

    if (fcntl(fd_, F_SETFL, flag) < 0) {
        util::err_print("could not set fd[%d] flag", fd_);
    }
}

ssize_t TCPClient::send(const char* buf, size_t len) {
    return ::send(fd_, buf, len, 0);
}

ssize_t TCPClient::recv(char* buf, size_t len, int flags) {
    ssize_t ret;

    while ((ret = ::recv(fd_, buf, len, flags)) < 0) {
        if (errno == EINTR)
            continue;
        else {
            if (errno != EAGAIN) {
                util::err_print("fd[%d] received error", fd_);
            }
            break;
        }
    }

    return ret;
}

ssize_t TCPClient::recv(char* buf, size_t len) {
    return recv(buf, len, 0);
}

ssize_t TCPClient::peek() {
    return recv(NULL, 0, MSG_PEEK);
}

int SSLClient::rsa_encrypt(const std::string& pubkey,
                           unsigned char* data,
                           size_t data_len,
                           unsigned char* buf) {
    BIO* bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        assert(false);
        return 0;
    }

    BIO_write(bio, pubkey.c_str(), static_cast<int>(pubkey.size()));

    RSA* rsa = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if (rsa == NULL) {
        assert(false);
        return 0;
    }

    int r = RSA_public_encrypt(static_cast<int>(data_len),
                               data, buf, rsa, RSA_PKCS1_OAEP_PADDING);
    if (r <= 0) {
        assert(false);
        return 0;
    }

    RSA_free(rsa);

    return r;
}

bool SSLClient::init_security(const std::string& pubkey) {
    if (security_state_ != SecureState::kInit) {
        return true;
    }

    generate_key();

    unsigned char buf[1024];
    auto len = rsa_encrypt(pubkey, (unsigned char*)security_key_, sizeof(security_key_), buf);
    if (len > 0) {
        util::OPacket pkt;
        pkt.setMsgId(MSG_C2S_SECURITY_INIT);
        pkt.setData(buf, len);
        send_packet(pkt);

        security_state_ = SecureState::kInProgress;
    }

    return true;
}

void SSLClient::complete_security() {
    if (security_state_ != SecureState::kInProgress) {
        return;
    }

    security_state_ = SecureState::kDone;
}

void SSLClient::send_packet(util::OPacket& pkt) {
    if (security_state_ == SecureState::kDone) {
        pkt.encode(security_key_);
    }

    pkt.wind();
    send(pkt.getBuf(), pkt.getBufSize());
}

}
