#include <iostream>
#include <stdint.h>

class Solution {
public:
    int reverse(int x) {
        long y = 0;
        while (x != 0) {
            y = (y * 10) + (x % 10);
            if (y > 2147483647 || y < -2147483648)
                return 0;
            x /= 10;
        }

        return static_cast<int>(y);
    }
};

int main() {
    Solution s;

    std::cout << s.reverse(123) << std::endl;
    std::cout << s.reverse(-123) << std::endl;
    std::cout << s.reverse(3) << std::endl;
    std::cout << s.reverse(1534236469) << std::endl;

    return 0;
}
