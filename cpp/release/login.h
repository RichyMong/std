#ifndef ROBOT_RELEASE_LOGIN_H
#define ROBOT_RELEASE_LOGIN_H

#include "base.h"
#include "packet.h"
#include "epoll.h"
#include <string>

namespace release {

class Login : public SSLClient, public util::EventHandler {
public:
    Login(const std::string& name,
          const std::string& passwd,
          const std::string& lang,
          const std::string& env)
        : username_(name), passwd_(passwd), lang_(lang), env_(env) {
    }

    bool login();

    void logout();

    void get_token(const std::string& gameid);

    void random_action();

public:
    virtual void on_readable(util::Multiplex& epoll);

    virtual void on_writeable(util::Multiplex& epoll);

    virtual void on_peer_closed(util::Multiplex& epoll);

private:
    Login(const Login&);

    void proc_login_result(util::IPacket& pkt);

    void proc_token_result(util::IPacket& pkt);

    void proc_checkpin_result(util::IPacket& pkt);

    void proc_newpin_result(util::IPacket& pkt);

    void proc_kicked(util::IPacket& pkt);

    void proc_security(util::IPacket& pkt);

    void dispatch(util::IPacket& pkt);

    std::string   username_;
    std::string   passwd_;
    std::string   lang_;
    std::string   env_;
    std::string   cookie_;

    util::IPacket inpkt_;
};

}

#endif /* ROBOT_RELEASE_LOGIN_H */
