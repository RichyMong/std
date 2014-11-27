#include <iostream>
#include <string>
#include <type_traits>

template <typename... Types> class Factory;

template <typename F, typename... Types>
class Factory<F, Types...> : public Factory<F>, public Factory<Types...> {
public:
    using Factory<F>::create;
    using Factory<Types...>::create;
};

template <typename F>
class Factory<F> {
public:
    template <typename... Args>
    F *create(F, Args... args)
    {
        static_assert(std::is_constructible<F, Args...>::value, "cannot construct!");
        return new F { args... };
    }
};

int main()
{
    Factory<int, std::string> factory;
    int *p = factory.create(int(), 10);
    std::string *s = factory.create(std::string(), "abc");
    std::cout << *p << std::endl;
    std::cout << *s << std::endl;
}
