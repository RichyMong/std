// Single Number II
// Given an array of integers, every element appears three times except for one. Find that single one.
//
// Note:
// Your algorithm should have a linear runtime complexity. Could you implement it without using extra memory?
//
// https://leetcode.com/problems/single-number-ii/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int singleNumber(const vector<int>& nums) {
        int x = 0, y = 0;

        for (auto num : nums) {
            x = (num ^ x) & ~y;
            y = (num ^ y) & ~x;
        }

        return x;
    }

};

int main()
{
    cout << Solution::singleNumber({1, 1, 1, 3}) << endl;
    cout << Solution::singleNumber({1, 1, 1, 2, 3, 3, 3}) << endl;
}
