#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <semaphore.h>

using namespace std;

namespace {

template <typename T>
class Queue {
public:
    Queue(size_t waterMark) : waterMark_ { waterMark }, capacity_ {1111}
    {
        sem_init(&wsem_, 0, capacity_);
        sem_init(&rsem_, 0, 0);
    }

    void post(const T &obj)
    {
        sem_wait(&wsem_);
        lock_guard<mutex> lck {mutex_};
        queue_.push(obj);

        if (!((queue_.size() - 1) % waterMark_))
            sem_post(&rsem_);
    }

    std::vector<T> wait()
    {
        sem_wait(&rsem_);
        lock_guard<mutex> lck {mutex_};
        size_t count = std::min(waterMark_, queue_.size());
        vector<T> items;
        items.reserve(count);
        for (size_t i = 0; i != count; ++i) {
            auto x = queue_.front();
            items.push_back(x);
            queue_.pop();
            sem_post(&wsem_);
        }
        return items;
    }
private:
    size_t   capacity_;
    size_t   waterMark_;
    queue<T> queue_;
    mutex    mutex_;
    sem_t    rsem_;
    sem_t    wsem_;
};

Queue<int> tasks {100};

void consumer(size_t count)
{
    size_t total = 0;
    while (total != count) {
        auto items = tasks.wait();
        total += items.size();
        this_thread::sleep_for(chrono::microseconds {10});
    }
    cout << __func__ << ": I received " << total << "\n";
}

void productor(size_t count)
{
    for (size_t i = 0; i != count; ++i)
        tasks.post(i + 1);
    cout << __func__ << ": I sent " << count << "\n";
}

}

int main()
{
    size_t count = 100000;
    thread cons { consumer, count };
    thread prod { productor, count };
    prod.join();
    cons.join();
}
