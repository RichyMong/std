#include <iostream>
#include <set>

class Solution {
public:
    static bool isHappy(int n) {
        if (n < 0) {
            return false;
        }

        std::set<int> vars;

        uint64_t x = n;

        while (x != 1) {
            if (vars.find(x) != vars.end()) return false;

            vars.insert(x);

            uint64_t s = 0;
            while (x > 10) {
                auto tmp = x % 10;
                s += tmp * tmp;
                x /= 10;
            }

            x = s;
        }

        return x == 1;
    }
};

int main()
{
    std::cout << Solution::isHappy(19) << std::endl;
    std::cout << Solution::isHappy(12) << std::endl;
}
