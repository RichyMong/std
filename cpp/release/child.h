#ifndef ROBOT_RELEASE_CHILD_H
#define ROBOT_RELEASE_CHILD_H

#include "login.h"

namespace release {

class Child {
public:
    Child(const std::string user) : lclient(user) {

    }

private:
    Login lclient_;
};

}

#endif /* ROBOT_RELEASE_CHILD_H */
