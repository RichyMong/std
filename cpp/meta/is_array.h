#ifndef IS_ARRAY_H
#define IS_ARRAY_H

namespace ouou {

template <typename T, typename... Args>
struct is_array {
    static const bool value = false;
};

template <typename T, unsigned int N>
struct is_array<T[N]> {
    static const bool value = true;
};

}

#endif // IS_ARRAY_H
