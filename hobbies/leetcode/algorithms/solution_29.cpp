// Divide Two Integers - Divide two integers without using multiplication, division
// and mod operator.
//
// If it is overflow, return MAX_INT.
// https://leetcode.com/problems/divide-two-integers/
#include "solution.h"
#include <limits>
#include <cstdlib>

using namespace std;

class Solution {
public:
    static int divide(int dividend, int divisor) {
        if (!divisor || (dividend == std::numeric_limits<int>::min() && divisor == -1))
            return std::numeric_limits<int>::max();

        bool same_sign = (dividend > 0 && divisor > 0) ||
                         (dividend < 0 && divisor < 0);
        long result = 0;

        long long x = labs(dividend);
        long long y = labs(divisor);

        while (x >= y) {
            long long z = y, k = 1;
            while (x >= (z << 1)) {
                z <<= 1;
                k <<= 1;
            }
            result += k;
            x -= z;
        }

        return same_sign ? result : -result;
    }

};

int main()
{
    cout << Solution::divide(8, 3) << endl;
    cout << Solution::divide(8, 2) << endl;
    cout << Solution::divide(8, -3) << endl;
    cout << Solution::divide(8, -2) << endl;
    cout << Solution::divide(std::numeric_limits<int>::min(), -1) << endl;
    cout << Solution::divide(std::numeric_limits<int>::min(), 1) << endl;
}
