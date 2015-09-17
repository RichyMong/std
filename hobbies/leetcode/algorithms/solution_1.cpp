#include <iostream>
#include <vector>
#include <unordered_map>
#include "solution.h"

using namespace std;

class Solution {
public:
    static vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> waits;

        for (int i = 0; i < nums.size(); ++i) {
            if (waits.find(nums[i]) == waits.end()) {
                waits[target - nums[i]] = i;
            } else {
                return vector<int> { waits[nums[i]] + 1, i + 1 };
            }
        }

        return vector<int> {};
    }

};

int main()
{
    vector<int> nums { 2, 7, 11, 15 };
    pretty_print(Solution::twoSum(nums, 9));
    pretty_print(Solution::twoSum(nums, 18));
    pretty_print(Solution::twoSum(nums, 19));
    pretty_print(Solution::twoSum(nums, 22));
}

