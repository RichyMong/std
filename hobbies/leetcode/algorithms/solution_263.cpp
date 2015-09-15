#include <iostream>
#include <array>

class Solution {
public:
    static bool isUgly(unsigned number) {
        if (!number) {
            return false;
        }

        std::array<int, 3> factors = { 2, 3, 5 };

        for (auto e : factors) {
            while (number % e == 0) number /= e;
        }

        return number == 1;
    }
};

int main()
{
    std::cout << Solution::isUgly(1) << std::endl;
    std::cout << Solution::isUgly(2) << std::endl;
    std::cout << Solution::isUgly(4) << std::endl;
    std::cout << Solution::isUgly(6) << std::endl;
    std::cout << Solution::isUgly(8) << std::endl;
    std::cout << Solution::isUgly(9) << std::endl;
    std::cout << Solution::isUgly(14) << std::endl;
}
