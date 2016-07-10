// 3Sum Closest - Given an array S of n integers, find three integers in S such
// that the sum is closest to a given number, target. Return the sum of the three
// integers. You may assume that each input would have exactly one solution.
//
//     For example, given array S = {-1 2 1 -4}, and target = 1.
//
//    The sum that is closest to the target is 2. (-1 + 2 + 1 = 2).
//
// https://leetcode.com/problems/3sum-closest/
#include "solution.h"
#include <vector>
#include <limits>
#include <numeric>
#include <algorithm>
#include <cstdio>

using namespace std;

class Solution {
public:
    static int threeSumClosest(std::vector<int>& nums, int target) {
        std::sort(nums.begin(), nums.end());
        if (nums.size() < 3) return std::accumulate(nums.begin(), nums.end(), 0);
        int ans = std::numeric_limits<int>::max();
        int closest = ans;
        for (auto first = 0; first < nums.size() - 2; ++first) {
            int second = first + 1, third = nums.size() - 1;
            while (second < third) {
                int tmp = nums[first] + nums[second] + nums[third];
                if (tmp == target) return tmp;
                else if (tmp < target) {
                    ++second;
                } else {
                    --third;
                }
                if (abs(tmp - target) < ans) {
                    closest = tmp;
                    ans = abs(tmp - target);
                }
            }
        }
        return closest;
    }

};

int main()
{
    auto nums = std::vector<int> { -1, -4, 2, 1 };
    cout << Solution::threeSumClosest(nums, 1) << endl;
    nums = std::vector<int> { -1, -4, 2, 1, 5, 3, 2, 8, 9 };
    cout << Solution::threeSumClosest(nums, 9) << endl;
    nums = std::vector<int> { 1, 1, 1, 0 };
    cout << Solution::threeSumClosest(nums, -100) << endl;
    return 0;
}
