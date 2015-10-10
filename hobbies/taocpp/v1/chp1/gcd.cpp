#include <iostream>

unsigned gcd(unsigned m, unsigned n)
{
    for (auto r = m % n; r; r = m % n) {
        m = n;
        n = r;
    }
    return n;
}

unsigned gcd2(unsigned m, unsigned n)
{
    while (m != 0) {
        if (n >= m) {
            n -= m;
        } else {
            std::swap(m, n);
        }
    }
    return n;
}

int main()
{
    std::cout << gcd(3, 6) << std::endl;
    std::cout << gcd(3, 2) << std::endl;
    std::cout << gcd(4, 2) << std::endl;
    std::cout << gcd2(3, 6) << std::endl;
    std::cout << gcd2(3, 2) << std::endl;
    std::cout << gcd2(4, 2) << std::endl;
    return 0;
}
