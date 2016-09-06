#include <iostream>

struct S {
    int i_;

    constexpr S(int i) : i_ { i } {  }

    void inc(int i) { i_ += i; }
};

struct T {
    const int i_;

    T(int i) : i_ { i } {  }
};

int main(int argc, char** argv) {
    constexpr S s(2);
    constexpr int i = s.i_;
    T t(3);
    constexpr int y = t.i_;
}
