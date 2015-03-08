#ifndef ROBOT_RELEASE_UTIL_H
#define ROBOT_RELEASE_UTIL_H

#include <algorithm>
#include <random>
#include <cstdlib>
#include <assert.h>

typedef char int8;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;
typedef unsigned long long uint64;
typedef long long int64;

namespace util {

template <typename T, size_t N>
inline size_t array_size(const T (&a)[N]) {
    (void) a;
    return N;
}

template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, T>::type
random_between(T low, T high) {
    assert(low < high);
    auto size = high - low;
    std::random_device rd;
    int val = rd() % size;
    return low + val;
}

template <typename T>
typename std::enable_if<!std::is_integral<T>::value, T>::type
random_between(T low, T high) {
    size_t size = std::distance(low, high);
    std::advance(low, random_between((size_t)0, size));
    return low;
}

template <typename Container>
typename Container::value_type& random_choice(Container& c) {
// auto random_choice(Container& c) -> decltype(*std::begin(c)) { // c++11
    return *random_between(std::begin(c), std::end(c));
}

template <typename Container>
typename Container::value_type& random_choice(const Container& c) {
// auto random_choice(const Container& c) -> decltype(*std::begin(c)) { // c++11
    return *random_between(std::begin(c), std::end(c));
}

// side effect: these functions will print newline at the end.
extern void err_exit(bool error, const char* fmt, ...);

extern void err_print(const char* fmt, ...);

}


#endif /* ROBOT_RELEASE_UTIL_H */
