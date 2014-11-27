#include <iostream>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <chrono>

using namespace std;

namespace {

template <typename T>
class Queue {
public:
    Queue(size_t waterMark) : waterMark_ { waterMark }, capacity_ {1000} {}

    void post(const T &obj)
    {
        unique_lock<mutex> lck {mutex_};
        auto oldsize = queue_.size();
        queue_.push(obj);

        if (!oldsize || oldsize >= waterMark_)
            cond_.notify_one();
    }

    std::vector<T> wait()
    {
        unique_lock<mutex> lck {mutex_};
        cond_.wait(lck, [&](){ return !queue_.empty(); });
        size_t count = std::min(waterMark_, queue_.size());
        vector<T> items;
        items.reserve(count);
        for (size_t i = 0; i != count; ++i) {
            auto x = queue_.front();
            items.push_back(x);
            queue_.pop();
        }
        return items;
    }
private:
    size_t             capacity_;
    size_t             waterMark_;
    queue<T>           queue_;
    mutex              mutex_;
    condition_variable cond_;
};

Queue<int> tasks {100};

void consumer(size_t count)
{
    while (count > 0) {
        auto items = tasks.wait();
        cout << "I got " << items.size() << " elements.\n";
        count -= items.size();
    }
}

void productor(size_t count)
{
    for (size_t i = 0; i != count; ++i)
        tasks.post(i + 1);
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
