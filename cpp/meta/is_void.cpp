#include <iostream>
#include "is_void.h"

class X { };

int main() {
    std::cout << ouou::is_void<void>::value << std::endl;
    std::cout << ouou::is_void<int>::value << std::endl;
    std::cout << ouou::is_void<X>::value << std::endl;
}
