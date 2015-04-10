#include <iostream>
#include <deque>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <unistd.h>

using namespace std;

static constexpr int MAX_LOG = 10000000;
static constexpr int thread_num = 4;

void worker_routine(FILE* file) {
    for (auto i = 0; i < MAX_LOG / thread_num; ++i) {
        string content = "this is a test just for test.\n";
        fwrite(content.c_str(), content.size(), 1, file);
    }
}

int main() {
    vector<thread> workers;

    auto file = fopen("log_no_thread.log", "w");

    for (auto i = 0; i != thread_num; ++i) {
        workers.push_back(thread { worker_routine, file });
    }

    for (auto i = 0; i != thread_num; ++i) {
        workers[i].join();
    }
    fflush(file);
    fsync(fileno(file));
}
