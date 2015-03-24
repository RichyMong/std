#include "master.h"
#include "util.h"
#include <iostream>

namespace server {

void Master::run() {
    auto cpu_cnt = util::cpu_cnt();
    std::cout << cpu_cnt << std::endl;

    for (auto i = 0; i != cpu_cnt; ++i) {
        workers_.push_back(std::make_shared<Worker>());
    }
}

}
