#include <iostream>
#include <thread>
#include <chrono>
#include <future>

int func(int i)
{
    std::cout << std::this_thread::get_id() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(i));

    return i;
}

int main()
{
    std::cout << std::this_thread::get_id() << std::endl;
    auto v = std::async(std::launch::async, func, 5);
    std::cout << v.get() << std::endl;
    return 0;
}
