#ifndef ROBOT_RELEASE_CHILD_H
#define ROBOT_RELEASE_CHILD_H

#include "login.h"
#include <memory>

namespace release {

class Child {
public:
    Child(const std::string user) {

    }

private:
    std::shared_ptr<Login> lclient_;
};

}

#endif /* ROBOT_RELEASE_CHILD_H */
