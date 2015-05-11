#ifndef ADD_CONST_REF_H
#define ADD_CONST_REF_H

namespace ouou {

template <typename T>
struct add_const_ref {
    using type = const T&;
};

template <typename T>
struct add_const_ref<T&> {
    using type = T;
};

}

#endif // ADD_CONST_REF_H
