// 4Sum Given an array S of n integers, are there elements a, b, c, and d in S
// such that a + b + c + d = target? Find all unique quadruplets in the array
// which gives the sum of target.
// Note:
// Elements in a quadruplet (a,b,c,d) must be in non-descending order. (ie, a  b  c  d)
// The solution set must not contain duplicate quadruplets.
//
//     For example, given array S = {1 0 -1 0 -2 2}, and target = 0.
//
//     A solution set is:
//     (-1,  0, 0, 1)
//     (-2, -1, 1, 2)
//     (-2,  0, 0, 2)
//
// https://leetcode.com/problems/4sum/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static vector<vector<int>> fourSum(vector<int>& nums, int target) {
        vector<vector<int>> result;

        sort(nums.begin(), nums.end());

        const int s = nums.size();
        for (int i = 0; i < s; ++i) {
            auto except_i = target - nums[i];
            for (int j = i + 1; j < s; ++j) {
                auto except_ij = except_i - nums[j];
                auto k = j + 1;
                auto m = s - 1;
                while (k < m) {
                    auto sum = nums[k] + nums[m];
                    if (sum < except_ij) ++k;
                    else if (sum > except_ij) --m;
                    else {
                        vector<int> answer {nums[i], nums[j], nums[k], nums[m]};
                        result.push_back(answer);
                        while (k < m && nums[k] == answer[2]) ++k;
                        while (k < m && nums[m] == answer[3]) --m;
                    }
                }
                while (j < s - 1 && nums[j] == nums[j + 1]) ++j;
            }
            while (i < s - 1 && nums[i] == nums[i + 1]) ++i;
        }

        for (auto &x : result) {
            for (auto i : x)
                cout << i << ' ';
            cout << endl;
        }

        return result;
    }
};

int main()
{
    vector<int> vec { -1, 0, 1, 0, 2, -2};
    Solution::fourSum(vec, 0);
    cout << endl;
    vector<int> vec2 { 1, -1, -1, 0 };
    Solution::fourSum(vec2, -1);
    vector<int> vec3 { -493,-490,-485,-474,-450,-413,-408,-368,-366,-361,-342,-324,-296,-250,-173,-142,-137,-121,-100,-53,-17,18,42,55,80,86,109,115,125,141,144,151,175,212,234,252,327,329,342,389,393,413,423,427,464,481,483,486,492,493 };
    Solution::fourSum(vec3, 4773);
}
