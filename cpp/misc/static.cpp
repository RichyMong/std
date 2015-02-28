#include <iostream>

class StaticTest {
public:
    void func() const;
};

void StaticTest::func() const
{
    static int ncall;
    ++ncall;
    std::cout << "address: " << &ncall << ", value: " << ncall << std::endl;
}

int main()
{
    StaticTest a, b;
    a.func();
    b.func();
}
