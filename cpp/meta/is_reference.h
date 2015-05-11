#ifndef IS_REFERENCE_H
#define IS_REFERENCE_H

namespace ouou {

template <typename T>
struct is_reference {
    static const bool value = false;
};

template <typename T>
struct is_reference<T&> {
    static const bool value = true;
};

}

#endif // IS_REFERENCE_H
