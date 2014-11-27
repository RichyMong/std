#include <iostream>

class A {
public:
    explicit A(int x) : val_ { x } { }
private:
    int val_;
};

A func(int val)
{
    return A {val};
}

int main()
{
    auto x = func(10);
    A y(11);
}
