#include <iostream>
#include <thread>
#include <chrono>

namespace {

struct guarded_thread : public std::thread {
    using std::thread::thread;
    ~guarded_thread() { if (joinable()) join(); }
};

void tick(int n)
{
    for (int i = 0; i != n; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds {1});
        std::cout << __FILE__ << ":" << __LINE__ << " --- " << i + 1 << std::endl;
    }
}

}

int main()
{
    std::cout << "Number of cores: " << std::thread::hardware_concurrency()
              << std::endl;

    guarded_thread gt { tick, 10 };

    return 0;
}
