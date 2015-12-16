// Divide Two Integers - Divide two integers without using multiplication, division
// and mod operator.
//
// If it is overflow, return MAX_INT.
// https://leetcode.com/problems/divide-two-integers/
#include "solution.h"
#include <limits>

using namespace std;

class Solution {
public:
    static int divide(int dividend, int divisor) {
        if (!dividend || !divisor) return 0;

        bool same_sign = (dividend > 0 && divisor > 0) ||
                         (dividend < 0 && divisor < 0);
        long result = 0;

        if (dividend > 0) dividend = -dividend;
        if (divisor > 0) divisor = -divisor;

        for (int x = 0; ; x += divisor) {
            if (++result == std::numeric_limits<int>::max())
                return result;

            if (x == dividend ||
                x == std::numeric_limits<int>::min() ||
                x == std::numeric_limits<int>::max())
                break;
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
