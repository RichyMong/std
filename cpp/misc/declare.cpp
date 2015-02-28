#include <iostream>

namespace {
    inline int* f(double) {
        std::cout << "f(double)" << std::endl;
        return nullptr;
    }

    inline int* g(double) {
        std::cout << "g(double)" << std::endl;
        return nullptr;
    }

    // return int *(*((*)[2]))(double)
    int* (*((*func(char))[2]))(double) {
        static int* (*array[2])(double) = {&f, &g};

        return &array;
    }
}

int main() {
    double ai[2] = { 1.0, 2.0 };
    double (*pi)[2] = &ai;

    int* (*array[2])(double) = {&f, &g};

    // p was a pointer to array of functions pointers
    int* (*((*pfa)[2]))(double) = &array; // int *(*((*)[2]))(double)
    ((*pfa)[0])((*pi)[0]);
    pfa = func('a');
    ((*pfa)[1])((*pi)[1]);

    (void) pi;
}
