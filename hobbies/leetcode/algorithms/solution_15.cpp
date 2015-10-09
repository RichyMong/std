// 3Sum - Given an array S of n integers, are there elements a, b, c in S such
// that a + b + c = 0? Find all unique triplets in the array which gives the
// sum of zero.
// Note:
//      Elements in a triplet (a,b,c) must be in non-descending order. (ie, a  b  c)
//      The solution set must not contain duplicate triplets.

// For example, given array S = {-1 0 1 2 -1 -4},
// A solution set is:
// (-1, 0, 1)
// (-1, -1, 2)

// https://leetcode.com/problems/3sum/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static vector<vector<int>> threeSum(vector<int>& nums) {
        sort(nums.begin(), nums.end());

        vector<vector<int>> result;

        int prev_i = -1;
        for (int i = 0; i < nums.size(); ++i) {
            if (nums[i] > 0) break;

            if (prev_i != -1 && nums[prev_i] == nums[i])
                    continue;
                prev_i = i;

            int prev_j = -1;
            for (int j = i + 1; j < nums.size(); ++j) {
                if (nums[i] + nums[j] > 0) break;

                if (prev_j != -1 && nums[prev_j] == nums[j])
                    continue;
                prev_j = j;

                int prev_k = -1;
                for (int k = j + 1; k < nums.size(); ++k) {
                    if (prev_k != -1 && nums[prev_k] == nums[k])
                        continue;
                    prev_k = k;

                    auto prev_s = nums[i] + nums[j] + nums[k];
                    if (prev_s > 0) break;
                    if (prev_s == 0) result.push_back(vector<int>{ nums[i], nums[j], nums[k]});
                }
            }
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
    vector<int> vec { -1, 0, 1, 2, -1, -2};
    Solution::threeSum(vec);
    cout << endl;
    vector<int> vec2 { 1, -1, -1, 0 };
    Solution::threeSum(vec2);
}
