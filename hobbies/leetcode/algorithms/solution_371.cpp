// Sum of Two Integers - Calculate the sum of two integers a and b, but you are
// not allowed to use the operator + and -.
// Example:
// Given a = 1 and b =
// 2, return 3.
// Credits:Special thanks to @fujiaozhu for adding this problem
// and creating all test cases.
// https://leetcode.com/problems/sum-of-two-integers/
#include "solution.h"

using namespace std;

class Solution {
public:
    int getSum(int a, int b) {
        if ((a & b) == 0) return a ^ b;
        return getSum((a & b) << 1, a ^ b);
    }
};

int main()
{
    Solution s;
    cout << s.getSum(3, 5) << endl;
    cout << s.getSum(2, 2) << endl;
    cout << s.getSum(63, 127) << endl;
    cout << s.getSum(512, -512) << endl;
}
