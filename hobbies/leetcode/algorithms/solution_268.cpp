// Missing Number
// Given an array containing n distinct numbers taken from 0, 1, 2, ..., n, find the one that is missing from the array.
// For example,
// Given nums = [0, 1, 3] return 2.
//
// Note:
// Your algorithm should run in linear runtime complexity. Could you implement it using only constant extra space complexity?
// Credits:Special thanks to @jianchao.li.fighter for adding this problem and creating all test cases.
// https://leetcode.com/problems/missing-number/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int missingNumber(const vector<int>& nums) {
        int missing_number = nums.size();

        for (auto i = 0; i < nums.size(); ++i) {
            missing_number ^= (i ^ nums[i]);
        }

        return missing_number;
    }

};

int main()
{
    cout << Solution::missingNumber({0, 1, 3}) << endl;
}
