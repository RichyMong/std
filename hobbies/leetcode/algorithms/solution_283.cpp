#include <iostream>
#include <vector>
#include <algorithm>
#include "solution.h"

using namespace std;

class Solution {
public:
    static void moveZeroes(vector<int>& nums) {
        int pos = 0;
        for (int i = 0; i < nums.size(); ++i) {
            if (nums[i]) nums[pos++] = nums[i];
        }
        while (pos < nums.size())
            nums[pos++] = 0;
    }
    static void moveZeroes2(vector<int>& nums) {
        auto first = find_if(nums.begin(), nums.end(), [](int x) { return x == 0; });
        auto pos = copy_if(first, nums.end(), first,
                           [](int x) { return x != 0; });
        fill(pos, nums.end(), 0);
    }
};

int main()
{
    vector<int> v { 0, 1, 0, 3, 12 };
    pretty_print(v);
    Solution::moveZeroes(v);
    pretty_print(v);
    vector<int> v2 { 0, 1, 0, 3, 12 };
    Solution::moveZeroes2(v2);
    pretty_print(v2);
}
