#include <iostream>
#include <thread>

int main()
{
    std::cout << "Number of cores: " << std::thread::hardware_concurrency() << std::endl;

    return 0;
}
