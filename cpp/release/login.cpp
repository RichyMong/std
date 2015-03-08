#include "login.h"
#include "proto.h"
#include <cstdio>
#include <string.h>
#include <map>

namespace release {

bool Login::login() {
    util::OPacket pkt;

    pkt.setMsgId(MSG_C2L_LOGIN);

    pkt << username_ << passwd_ << lang_ << env_ << 0; // 0 for COMMON_USER

    send_packet(pkt);

    return true;
}

void Login::logout() {
    util::OPacket pkt;

    pkt.setMsgId(MSG_C2L_LOGOUT);

    send_packet(pkt);
}

void Login::get_token(const std::string& gameid) {
    util::OPacket pkt;

    pkt.setMsgId(MSG_C2L_GET_TOKEN);

    pkt << 0 << gameid; // 1 for email login, 0 for common login

    send_packet(pkt);

    return;
}

void Login::dispatch(util::IPacket& pkt) {
    typedef std::map<int, void (Login::*)(util::IPacket&)> OperMap;

    OperMap kOperMap = {
        { MSG_S2C_SECURITY_INIT,    &Login::proc_security },
        { MSG_S2C_INIT_FINAL,       &Login::proc_security },
        { MSG_L2C_LOGIN_RESULT,     &Login::proc_login_result },
        { MSG_L2C_GET_TOKEN_RESULT, &Login::proc_token_result },
        { MSG_L2C_KICK,             &Login::proc_kicked },
    };

    if (security_state_ == SecureState::kDone) {
        pkt.decode(security_key_);
    }

    auto it = kOperMap.find(pkt.getMsgId());
    if (it != kOperMap.end()) {
        try {
            (this->*it->second)(pkt);
        } catch (util::PacketOverread&) {
            fprintf(stderr, "packet overread, message id: 0x%x\n", pkt.getMsgId());
        }
    } else {
        fprintf(stderr, "unknown message id %hu\n", pkt.getMsgId());
    }
}

void Login::on_readable(util::Multiplex& epoll) {
    char buf[PACKET_SIZE];
    auto len = 0;

    do {
        len = recv(buf, sizeof(buf));
        if (len == 0) {
            closefd();
            break;
        }

        auto nbuf = len;
        do {
            nbuf -= inpkt_.append(buf, nbuf);
            if (inpkt_.is_complete()) {
                dispatch(inpkt_);
                inpkt_.reset();
            }
        } while (nbuf > 0);
    } while (len == sizeof(buf));

    if (len == 0) {
        epoll.modify(sockfd(), EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | EPOLLET, this);
    }
}

void Login::on_writeable(util::Multiplex& epoll) {
    int       error;
    socklen_t len = sizeof(error);

    if (getsockopt(sockfd(), SOL_SOCKET, SO_ERROR, &error, &len) == 0) {
        if (error == 0) {
            epoll.modify(sockfd(), EPOLLIN | EPOLLRDHUP | EPOLLONESHOT, this);
        } else {
            fprintf(stderr, "could not connect: %s\n", strerror(error));
        }
    }
}

void Login::on_peer_closed(util::Multiplex& epoll) {
    epoll.remove(sockfd());
}

void Login::proc_security(util::IPacket& pkt) {
    if (pkt.getMsgId() == MSG_S2C_SECURITY_INIT) {
        std::string pubkey;
        pkt >> pubkey;
        init_security(pubkey);
    } else {
        complete_security();
        login();
     }
}

void Login::proc_login_result(util::IPacket& pkt) {
    enum {
        kFailure = 0,
        kSuccess,
    };

    char flag, result;

    pkt >> flag >> result;
    if (flag == kSuccess) {
        std::string sid, nname, uname;
        pkt >> sid >> nname >> uname >> cookie_;
    } else {
        fprintf(stderr, "%s login failed: %d\n", username_.c_str(), result);
    }
}

void Login::proc_token_result(util::IPacket& pkt) {
    std::string token;
    pkt >> token;
    fprintf(stderr, "token result: %s\n", token.c_str());
}

void Login::proc_kicked(util::IPacket& pkt) {
    fprintf(stderr, "was kicked\n");
    closefd();
}

void Login::proc_checkpin_result(util::IPacket& pkt) {

}

void Login::proc_newpin_result(util::IPacket& pkt) {

}

void Login::random_action() {
    int val = rand();

    get_token("pwe");
    if ((val & 0x0F) == 0) {

    } else if ((val & 0x03FF) == 0) {

    } else if ((val & 0x07FF) == 0) {

    }
}

}
