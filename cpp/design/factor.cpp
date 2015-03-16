#include <iostream>

template <size_t N>
struct Factor : public Factor<N - 1> {
    static constexpr size_t value = N * Factor<N - 1>::value;
};

template <>
struct Factor<0> {
    static constexpr size_t value = 1;
};

int main() {
    std::cout << "Factor<3>: "  << Factor<3>::value << std::endl;
}
