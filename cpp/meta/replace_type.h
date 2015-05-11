#ifndef REPLACE_TYPE
#define REPLACE_TYPE

namespace ouou {

template <typename Target, typename From, typename To>
struct replace_type;

template <typename T, typename U>
struct replace_type<T*, T, U> {
    using type = U*;
};

template <typename T, typename U>
struct replace_type<T&, T, U> {
    using type = U&;
};

template <typename T, typename U, int N>
struct replace_type<T[N], T, U> {
    using type = U[N];
};

template <typename T, typename U>
struct replace_type<T&(T&), T, U> {
    using type = U& (U&);
};

}

#endif // REPLACE_TYPE
