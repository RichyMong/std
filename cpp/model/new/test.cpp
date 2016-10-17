#include "new.h"
#include <memory>
#include <iostream>

#ifdef DEBUG
#define new DEBUG_NEW
#endif

struct X {
    long l;
    int i;
};

int main() {
    auto spi = std::make_shared<int>(5);
    auto spx = std::make_shared<X>();
    int* pi = new int(5);
    int* pa = new int[5];
    X* px = new X();
    delete px;
    delete pa;
    delete pi;
}
