// Next Permutation
// Implement next permutation, which rearranges numbers into the lexicographically next greater permutation of numbers.
//
// If such arrangement is not possible, it must rearrange it as the lowest possible order (ie, sorted in ascending order).
//
// The replacement must be in-place, do not allocate extra memory.
//
// Here are some examples. Inputs are in the left-hand column and its corresponding outputs are in the right-hand column.
// 1,2,3  1,3,2
// 3,2,1  1,2,3
// 1,1,5  1,5,1
//
// https://leetcode.com/problems/next-permutation/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static void nextPermutation(vector<int>& nums) {
        auto first = nums.begin(), last = nums.end();

        auto i = first;

        if (++i == last) return;

        i = last;
        --i;

        while (true) {
            auto ii = i;
            --i;
            if (*i < *ii) {
                auto j = last;
                while (*--j <= *i) { }
                iter_swap(i, j);
                reverse(ii, last);
                return;
            }
            if (i == first) {
                reverse(first, last);
                return;
            }
        }
    }

    static void nextPermutation2(vector<int>& nums) {
        auto i = is_sorted_until(nums.rbegin(), nums.rend());
        if (i != nums.rend())
            swap(*i, *upper_bound(nums.rbegin(), i, *i));
        reverse(nums.rbegin(), i);
    }
};

int main()
{
    vector<int> nums { 1, 2, 3 };
    Solution::nextPermutation(nums);
    pretty_print(nums);

    nums = { 3, 2, 1 };
    Solution::nextPermutation(nums);
    pretty_print(nums);

    nums = { 1, 5, 1 };
    Solution::nextPermutation2(nums);
    pretty_print(nums);

    nums = { 3, 4, 2, 1 };
    Solution::nextPermutation(nums);
    pretty_print(nums);

    nums = { 3, 5, 4, 2, 1 };
    Solution::nextPermutation2(nums);
    pretty_print(nums);
}
