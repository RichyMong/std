#include <iostream>
#include "is_function.h"

class X { };

int main() {
    using func = decltype(main);
    std::cout << ouou::is_function<func>::value << std::endl;
    std::cout << ouou::is_function<int>::value << std::endl;
    std::cout << ouou::is_function<X>::value << std::endl;
}
