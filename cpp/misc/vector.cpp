#include <iostream>
#include <vector>
#include <algorithm>

std::vector<int> f(size_t size)
{
    std::vector<int> v;
    v.reserve(size);
    for (size_t i = 0; i != size; ++i)
        v.push_back(10);

    return v;
}

void reserve_test()
{
    char buf[] = "abcdefg";
    std::vector<char> v;
    v.reserve(128);
    v.push_back('0');
    v.push_back('1');
    std::copy(buf, buf + sizeof(buf) - 1, std::back_inserter(v));
    std::cout << v.size() << ", capacity " << v.capacity() << std::endl;
}

int main()
{
    auto x = f(20);
    std::cout << x.size() << std::endl;
    reserve_test();
}
