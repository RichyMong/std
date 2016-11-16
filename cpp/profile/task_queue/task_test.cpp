#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <array>
#include <memory>
#include <cstdio>
#include <unistd.h>
#include "task_queue.h"

using namespace std;
using namespace std::chrono;

static size_t kTaskCount = 100000;
static auto count = 0;
static mutex complete_mutex;
static condition_variable complete_cond;

class Runnable
{
public:
    virtual void run() = 0;
};

struct CopyData {
    string name;
    vector<int> scores;
};

struct Data
{
    vector<int> scores;
    string name;
};

class MultiImpl;

template <typename T>
class Task : public Runnable
{
public:
    void run() override;

    Task(MultiImpl* impl, T&& data)
        : impl_(impl), data_(std::forward<T>(data))
    {
    }

    Task(MultiImpl* impl, const T& data)
        : impl_(impl), data_(data)
    {
    }


private:
    MultiImpl* impl_;
    T data_;
};

class MultiImpl
{
public:
    template <typename T>
    void receive(const Task<T>&)
    {
        if (first_tp_ == time_point<steady_clock>()) {
            first_tp_ = steady_clock::now();
        }

        if (++rcvd_cnt_ == kTaskCount) {
            auto d = duration_cast<microseconds>(steady_clock::now() - first_tp_);
            fprintf(stdout, "%lld: used %lld microseconds\n", pthread_self(),
                    d.count());
            {
                lock_guard<mutex> guard(complete_mutex);
                ++count;
            }
            complete_cond.notify_one();
        }
    }

private:
    steady_clock::time_point first_tp_;
    size_t rcvd_cnt_ = 0;
};

template <typename T>
void Task<T>::run()
{
    impl_->receive(*this);
}

int main(int argc, char* argv[])
{
    if (argc > 1) kTaskCount = atoi(argv[1]);

    typedef Data DataType;
    vector<util::TaskQueue<Task<DataType>>> tqs(8);
    vector<MultiImpl> impl(tqs.size());
    for (size_t i = 0; i < tqs.size(); i++) {
        tqs[i].start();
    }

    std::array<int, 12> sizes { 10000, 12000, 15000, 20000, 25000, 1000, 2500, 1000, 6000, 10000, 900, 16 };

    for (size_t i = 0; i < kTaskCount; ++i) {
        DataType data;
        auto size = sizes[i % sizes.size()];
        data.scores.reserve(size);
        for (int j = 0; j < size; j++) {
           data.scores.push_back(j + 1);
        }
        data.name = string(size, '*');
        for (size_t k = 0; k < tqs.size(); k++) {
            tqs[k].push(Task<DataType>(&impl[k], data));
        }
    }

    while (count < 8) {
        unique_lock<mutex> ulock(complete_mutex);
        if (count < 8) {
            complete_cond.wait(ulock);
        }
        ulock.unlock();
    }

    for (size_t i = 0; i < tqs.size(); i++) {
        tqs[i].wait();
    }

    return 0;
}
