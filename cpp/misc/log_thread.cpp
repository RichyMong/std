#include <iostream>
#include <deque>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

using namespace std;

static constexpr int MAX_LOG = 10000000;
static constexpr int thread_num = 4;

deque<string> logs;
mutex log_mutex;
condition_variable cv;

void worker_routine() {
    for (auto i = 0; i < MAX_LOG / thread_num; ++i) {
        unique_lock<mutex> lck { log_mutex };
        logs.push_back(string("this is a test just for test."));
        cv.notify_one();
    }
}

void log_routine() {
    auto file = fopen("log_thread.log", "w");
    for (auto i = 0; i < MAX_LOG; ++i) {
        string content;
        {
            unique_lock<mutex> lck { log_mutex };
            if (logs.empty()) {
                cv.wait(lck);
            }

            content = logs.front();
            logs.pop_front();
        }
        fprintf(file, "%s\n", content.c_str());
    }

}

int main() {
    vector<thread> workers;

    thread log_thread { log_routine };
    for (auto i = 0; i != thread_num; ++i) {
        workers.push_back(thread { worker_routine });
    }

    for (auto i = 0; i != thread_num; ++i) {
        workers[i].join();
    }

    log_thread.join();
}
