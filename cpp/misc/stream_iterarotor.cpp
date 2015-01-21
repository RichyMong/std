#include <iostream>
#include <iterator>
#include <algorithm>

int main()
{
    using in = std::istream_iterator<int>;

    std::for_each(in(std::cin), in(), [](int x) { std::cout << x * 3 << std::endl; });

    return 0;
}
