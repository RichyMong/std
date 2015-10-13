// Remove Duplicates from Sorted Array II
// Follow up for "Remove Duplicates":
// What if duplicates are allowed at most twice?
//
// For example,
// Given sorted array nums = [1,1,1,2,2,3],
//
// Your function should return length = 5, with the first five elements of nums
// being 1, 1, 2, 2 and 3. It doesn't matter what you leave beyond the new length.
//
// https://leetcode.com/problems/remove-duplicates-from-sorted-array-ii/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int removeDuplicates(vector<int>& nums) {
        int pos = 0;
        int size = nums.size();

        for (int i = 0; i < size; ++i) {
            nums[pos++] = nums[i];

            auto tmp = i;
            while (i < size - 1 && nums[i + 1] == nums[i])
                ++i;

            if (i != tmp)
                nums[pos++] = nums[tmp];
        }

        return pos;
    }

    static int removeDuplicates2(vector<int>& nums) {
        int count = 0;
        int size = nums.size();

        for (int i = 2; i < size; ++i) {
            if (nums[i] != nums[i - 2 - count]) count++;
            else nums[i - count] = nums[i];
        }

        return size - count;
    }

    static int removeDuplicates3(vector<int>& nums) {
        int i = 0;

        for (int num : nums) {
            if (i < 2 || num != nums[i - 2]) {
                swap(nums[i++], num);
            }
        }

        return i;
    }
};

int main()
{
    vector<int> nums { 1, 1, 1, 2, 2, 3 };
    cout << Solution::removeDuplicates3(nums) << endl;
    pretty_print(nums);

    nums = { 1, 1, 1, 1, 1, 1 };
    cout << Solution::removeDuplicates3(nums) << endl;
    pretty_print(nums);

    nums = { 1, 1, 1, 1, 1, 2 };
    cout << Solution::removeDuplicates3(nums) << endl;
    pretty_print(nums);
}
