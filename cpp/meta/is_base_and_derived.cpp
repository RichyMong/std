#include <iostream>
#include "is_base_and_derived.h"

class B { };

class D : public B {
    D(int, int);
};

int main() {
    std::cout << ouou::is_base_and_derived<short, int>::value << std::endl;
    std::cout << ouou::is_base_and_derived<int, int*>::value << std::endl;
    std::cout << ouou::is_base_and_derived<B, D>::value << std::endl;
}
