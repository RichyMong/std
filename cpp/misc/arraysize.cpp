#include <iostream>
#include <string>
#include <set>

template <typename T, size_t N>
inline size_t array_size(const T (&x)[N])
{
    return sizeof(x) /sizeof(x[0]);
}

int main()
{
    int ai[] = {1, 3, 5};
    const std::string si[] = {"abc", "def"};
    std::cout << array_size(ai) << std::endl;
    std::cout << array_size(si) << std::endl;
    std::set<std::string> sets(std::begin(si), std::end(si));
    for (auto it = sets.cbegin(); it != sets.cend(); ++it)
        std::cout << *it << std::endl;
    return 0;
}
