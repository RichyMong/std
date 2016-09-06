#include <iostream>
#include <utility>

class X {
public:
    X() = default;
    X(const X&) { std::cout << "copy constructor\n"; }
    X(X&&) { std::cout << "move constructor\n"; }
};

int main()
{
    int a = 2;
    int&& b = std::move(a);
    std::cout << &a << ", " << &b << std::endl;
    X x1;
    X&& x2 = std::move(x1);
    std::cout << "constructing x3\n";
    X x3(x2);
    std::cout << "constructing x4\n";
    X x4(std::move(x1));
    return 0;
}
