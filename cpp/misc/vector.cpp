#include <iostream>
#include <vector>

std::vector<int> f(size_t size)
{
    std::vector<int> v;
    v.reserve(size);
    for (size_t i = 0; i != size; ++i)
        v.push_back(10);

    return v;
}

int main()
{
    auto x = f(20);
    std::cout << x.size() << std::endl;
}
