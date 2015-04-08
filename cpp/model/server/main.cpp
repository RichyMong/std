#include "master.h"
#include <iostream>
#include <execinfo.h>

int main(int argc, char** argv) {
    auto log = std::make_shared<util::Log>(stdout);
    try {
        server::Master master(log);
        master.add_server(std::make_shared<server::Server>(12345, log));
        master.run();
    } catch (std::exception& e) {
        log->error("exception: %s", e.what());
    }

    return 0;
}
