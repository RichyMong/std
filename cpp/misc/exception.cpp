#include <iostream>

class Exception {
    virtual void test() final {}
};

void test_excep(int &x)
{
    if (x % 2 == 0)
        throw Exception();
    else
        x = 3;
}

void f(int& x)
{
    try {
        test_excep(x);
    } catch(Exception &e) {
        std::cout << "caught" << std::endl;
        x = 1;
    }
    std::cout << x << std::endl;
}

int main()
{
    int x = 2;
    f(x);
    f(x);
}
