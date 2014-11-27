#include <iostream>

struct NullType {};

template <typename T> struct Length;
template <typename T, typename U> struct TypeList;

template <>
struct Length<NullType> {
    enum { value = 0 };
};

template <typename T, typename U>
struct Length<TypeList<T, U>> {
    enum { value = 1 + Length<U>::value };
};

template <typename T, typename U>
struct TypeList {
    using Head = T;
    using Tail = U;
    static constexpr int length = Length<TypeList<T, U>>::value;
};

template <typename T, size_t N> struct Index;

template <typename T, typename U>
struct Index<TypeList<T, U>, 0U> {
    using type = T;
};

template <typename T, typename U, size_t N>
struct Index<TypeList<T, U>, N> : public Index<U, N - 1> {
};

int main()
{
    using Signed = TypeList<char, TypeList<short, TypeList<int, NullType>>>;

    std::cout << TypeList<char, NullType>::length << std::endl;
    std::cout << TypeList<char, TypeList<short, NullType>>::length << std::endl;
    std::cout << TypeList<char, TypeList<short, TypeList<int, NullType>>>::length << std::endl;

    std::cout << sizeof(Index<Signed, 1>::type) << std::endl;
    std::cout << sizeof(Index<Signed, 2>::type) << std::endl;
}
