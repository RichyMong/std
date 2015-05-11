#include "add_const_ref.h"
#include <type_traits>
#include <iostream>

int main() {
    std::cout << std::is_same<ouou::add_const_ref<int>::type, const int&>::value << std::endl;
    std::cout << std::is_same<ouou::add_const_ref<int&>::type, int>::value << std::endl;

    return 0;
}
