#ifndef ROBOT_RELEASE_LOGIN_H
#define ROBOT_RELEASE_LOGIN_H

#include "base.h"
#include <string>

namespace release {

class Login : public TCPClient {
public:
    Login(const std::string& user);

    bool login();

    void logout();

    void random_action() const;

private:
    Login(const Login&);

    std::string user_;
};

}

#endif /* ROBOT_RELEASE_LOGIN_H */
