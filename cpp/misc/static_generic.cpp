#include <iostream>
#include <typeinfo>

class StaticGeneric {
public:
    template <typename T>
    static void test(T t)
    {
        std::cout << "type is " << typeid(t).name() << std::endl;
    }

    static void test(short) {
    }

    static void test(char)
    {
    }
};

int main()
{
    int i = 1;
    short s = 2;
    char c = 'a';

    StaticGeneric::test(i);
    StaticGeneric::test(s);
    StaticGeneric::test(c);

    std::cout << (-2 >> 5) << std::endl;
    std::cout << (-1 >> 30) << std::endl;

    return 0;
}
