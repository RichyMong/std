#include <iostream>
#include <deque>
#include <thread>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>
#include <mutex>
#include <chrono>
#include <condition_variable>

using namespace std;

static constexpr int MAX_LOG = 10000000;

deque<string> logs;
mutex log_mutex;
condition_variable cv;

static mutex stdout_mutex;

void worker_routine(int log_cnt) {
    auto start_time = std::chrono::steady_clock::now();
    for (auto i = 0; i < log_cnt; ++i) {
        unique_lock<mutex> lck { log_mutex };
        logs.push_back(string("this is a test just for test."));
        cv.notify_one();
    }
    {
        auto period = std::chrono::steady_clock::now() - start_time;
        unique_lock<mutex> lock { stdout_mutex };
        cout << "log record count: " << log_cnt << ", time: "
             << chrono::duration_cast<chrono::microseconds>(period).count()
             << " microseconds\n";
    }
}

void log_routine(int log_cnt) {
    auto file = fopen("log_thread.log", "w");
    for (auto i = 0; i < log_cnt; ++i) {
        string content;
        {
            unique_lock<mutex> lck { log_mutex };
            cv.wait(lck, [] { return !logs.empty(); });

            content = logs.front();
            logs.pop_front();
        }
        fprintf(file, "%s\n", content.c_str());
    }

}

int main(int argc, char* argv[]) {
    auto log_cnt = (argc > 1 ? atoi(argv[1]) : MAX_LOG);

    vector<thread> workers;
    auto thread_num = (int) std::thread::hardware_concurrency();
    auto log_cnt_per_thd = (log_cnt + thread_num - 1) / thread_num;

    thread log_thread { log_routine, log_cnt };
    auto use_cnt = 0;
    while (use_cnt < ((log_cnt / log_cnt_per_thd) * log_cnt_per_thd)) {
        workers.push_back(thread { worker_routine, log_cnt_per_thd });
        use_cnt += log_cnt_per_thd;
    }

    if (use_cnt < log_cnt) {
        workers.push_back(thread { worker_routine, log_cnt - use_cnt });
    }

    for_each(workers.begin(), workers.end(), mem_fn(&std::thread::join));

    log_thread.join();
}
