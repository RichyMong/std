#include <iostream>

class B {
public:
    B() { std::cout << __func__ << std::endl; }
};

class X {
public:
    X() : y_ { x_ + 1} { std::cout << y_ << std::endl; }

private:
    int x_ = 1;
    int y_ = 3;
    B* b_ = new B();
};

int main()
{
    X x;
}
