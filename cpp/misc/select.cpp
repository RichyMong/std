#include <iostream>
#include <cassert>

template <int N, typename... Args> struct Select;

template <int N, typename First, typename... Args>
struct Select<N, First, Args...> : Select<N - 1, Args...> {
    static_assert(N < sizeof...(Args), "Select: N too large!");
};

template <typename First, typename... Args>
struct Select<0, First, Args...> {
    using type = First;
};

struct Error {
    Error() { assert(false); }
};

template <int N>
struct Integer {
    using type = typename Select<N, Error, signed char, short, Error, int, Error, Error, Error, long long>::type;
};

int main()
{
    std::cout << sizeof(Integer<1>::type) << std::endl;
    std::cout << sizeof(Integer<4>::type) << std::endl;
}
