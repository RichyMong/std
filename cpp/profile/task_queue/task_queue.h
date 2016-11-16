# pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

namespace util
{

template <typename T>
class TaskQueue {
public:
    explicit TaskQueue(size_t limit = 0)
        : limit_ { limit }, running_ { false }
    {
    }

    void push(const T& task) {
        std::unique_lock<std::mutex> ulock(mutex_);
        bool empty = queue_.empty();
        while (running_ && limit_ && queue_.size() >= limit_) {
            free_cond_.wait(ulock);
        }

        if (running_) {
            queue_.push(task);
            ulock.unlock();
            if (empty) {
                data_cond_.notify_one();
            }
        } else {
            ulock.unlock();
        }
    }

    void start()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        if (!running_) {
            running_ = true;
            worker_thread_ = std::move(std::thread(&TaskQueue<T>::work_routine, this));
        }
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> guard(mutex_);
            running_ = false;
        }

        data_cond_.notify_all();
        free_cond_.notify_all();
    }

    void wait()
    {
        stop();
        worker_thread_.join();
    }

private:
    void work_routine()
    {
        while (running_) {
            std::unique_lock<std::mutex> ulock(mutex_);
            while (running_ && queue_.empty()) {
                data_cond_.wait(ulock);
            }

            if (!running_) {
                ulock.unlock();
                break;
            }

            std::queue<T> tmp(move(queue_));
            ulock.unlock();
            if (tmp.size() >= limit_) {
                free_cond_.notify_all();
            }

            while (!tmp.empty()) {
                tmp.back().run();
                tmp.pop();
            }
        }
    }

    std::thread worker_thread_;
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable free_cond_;
    std::condition_variable data_cond_;
    const size_t limit_;
    bool running_;
};

}
