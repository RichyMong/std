#include "binary.h"
#include <iostream>

int main() {
    std::cout << binary<101010>::value << std::endl;
    std::cout << binary<1101011>::value << std::endl;
    std::cout << binary<(int) 0xFFFFFFFF>::value << std::endl;
    std::cout << binary<-1011>::value << std::endl;
    std::cout << binary<-100001>::value << std::endl;
    std::cout << binary_int<101010>::value << std::endl;
    std::cout << binary_int<(int) 0xFFFFFFFF>::value << std::endl;
    std::cout << binary_int<-100001>::value << std::endl;
    return 0;
}
