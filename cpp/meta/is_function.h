#ifndef IS_FUNCTION_H
#define IS_FUNCTION_H

namespace ouou {

template <typename T, typename... Args>
struct is_function {
    static const bool value = false;
};

template <typename T, typename... Args>
struct is_function<T(Args...)> {
    static const bool value = true;
};

}

#endif // IS_FUNCTION_H
