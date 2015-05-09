#ifndef META_BINARY_H
#define META_BINARY_H

#include <cstdlib>
#include <type_traits>

template <int n, int pos, typename = typename std::enable_if<(n % 10 < 2), void>::type>
struct binary_helper {
    static constexpr int value = (binary_helper<n / 10, pos +1>::value << 1) + (n % 10);
};

template <>
struct binary_helper<0, 31> {
    static constexpr int value = 0;
};

template <>
struct binary_helper<1, 31> {
    static constexpr int value = -1;
};

template <int n>
struct binary {
    static constexpr int value = binary_helper<n, 0>::value;
};

template <int n, typename = typename std::enable_if<(n % 10 < 2), void>::type>
struct binary_int {
    static constexpr int value = (binary_int<n / 10>::value << 1) + (n % 10);
};

template <>
struct binary_int<0> {
    static constexpr int value = 0;
};

#endif // META_BINARY_H
