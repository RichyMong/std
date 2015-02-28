#include <iostream>

class Base {
public:
    explicit Base(int n) : val_ {n} { }
    void show() const { std::cout << val_ << std::endl; }
private:
    int val_;
};

class Derived : public Base {
public:
    using Base::Base;
};

int main()
{
    Derived der { 20 };
    der.show();
}
