#include "master.h"
#include <iostream>
#include <execinfo.h>

int main(int argc, char** argv) {
    auto log = std::make_shared<util::Log>(stdout);
    try {
        server::Master<util::Epoll> master(log);
        master.run();
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }

    return 0;
}
