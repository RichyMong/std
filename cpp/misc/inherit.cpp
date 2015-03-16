#include <iostream>

class Base {
public:
    Base() : val_ {-1} {
        std::cout << "Base()" << std::endl;
    }

private:
    int val_;
};

class Derived : public Base {
public:
    Derived() : name { "test" } {

    }

private:
    const char* name;
};

int main() {
    Derived der;
}

