// first-missing-positive - Given an unsorted integer array, find the first missing
// positive integer.
//
// For example,
// Given [1,2,0] return 3,
// and [3,4,-1,1] return 2.
//
// Your algorithm should run in O(n) time and uses constant space.
// https://leetcode.com/problems/first-missing-positive
#include "solution.h"

using namespace std;

class Solution {
public:
    int firstMissingPositive(const vector<int>& nums) {
        int x = 0;
        for (size_t i = 0; i < nums.size(); i++) {
            x ^= i + 1;
            if (nums[i] > 0) {
                x ^= nums[i];
            }
        }

        for (size_t i = 0; i < nums.size(); i++) {
            if ((x & (i + 1)) == (i + 1)) {
                return i + 1;
            }
        }

        return 0;
    }
};

int main()
{
    Solution s;
    cout << s.firstMissingPositive(vector<int> { 1, 2, 0 }) << endl;
    cout << s.firstMissingPositive(vector<int> { 3, 4, -1, 1 }) << endl;
}
