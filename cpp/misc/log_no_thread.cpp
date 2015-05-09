#include <iostream>
#include <deque>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <unistd.h>
#include <chrono>

using namespace std;

static constexpr int LOG_CNT = 10000000;
static mutex stdout_mutex;

void worker_routine(FILE* file, int log_cnt) {
    auto start_time = std::chrono::steady_clock::now();
    for (auto i = 0; i < log_cnt; ++i) {
        string content = "this is a test just for test.\n";
        fwrite(content.c_str(), content.size(), 1, file);
    }
    {
        auto period = std::chrono::steady_clock::now() - start_time;
        unique_lock<mutex> lock { stdout_mutex };
        cout << "log record count: " << log_cnt << ", time: "
             << chrono::duration_cast<chrono::microseconds>(period).count()
             << " seconds\n";
    }
}

int main() {
    vector<thread> workers;
    auto thread_num = (int) std::thread::hardware_concurrency();

    auto file = fopen("log_no_thread.log", "w");

    for (auto i = 0; i != thread_num; ++i) {
        workers.push_back(thread { worker_routine, file, LOG_CNT / thread_num });
    }

    for (auto i = 0; i != thread_num; ++i) {
        workers[i].join();
    }
}
