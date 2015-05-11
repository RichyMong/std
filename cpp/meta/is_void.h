#ifndef IS_VOID_H
#define IS_VOID_H

namespace ouou {

template <typename T>
struct is_void {
    static const bool value = false;
};

template <>
struct is_void<void> {
    static const bool value = true;
};

template <typename T>
struct is_enum {
    static const bool value = false;
};

}

#endif // IS_VOID_H
