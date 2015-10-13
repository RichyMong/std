// Search in Rotated Sorted Array Suppose a sorted array is rotated at some
// pivot unknown to you beforehand. (i.e., 0 1 2 4 5 6 7 might become 4 5 6 7 0 1 2).
// You are given a target value to search. If found in the array return its
// index, otherwise return -1.
// You may assume no duplicate exists in the array.
// https://leetcode.com/problems/search-in-rotated-sorted-array/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int search(const vector<int>& nums, int target) {
        if (nums.size() == 0) return -1;

        int privot = -1;
        int low = 0, high = (int) nums.size() - 1;

        while (low < high - 1) {
            int mid = low + (high - low) / 2;
            if (nums[mid] > nums[mid + 1]) {
                privot = mid;
                break;
            } else if (nums[mid] > nums[high]) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }

        if (low < nums.size() - 1 && nums[low] > nums[low + 1]) privot = low;

        vector<int> rotated(nums.begin() + privot + 1, nums.end());
        rotated.insert(rotated.end(), nums.begin(), nums.begin() + privot + 1);

        low = 0, high = rotated.size() - 1;
        while (low < high) {
            int mid = low + (high - low) / 2;
            if (rotated[mid] == target) return mid;
            if (rotated[mid] < target) low = mid + 1;
            else high = mid - 1;
        }

        return rotated[low] == target ? low : -1;
    }

    static int bsearch(const vector<int>& nums, int target) {
        if (nums.size() == 0) return -1;

        int privot = -1;
        int low = 0, high = (int) nums.size() - 1;

        while (low <= high - 1) {
            int mid = low + (high - low) / 2;
            if (nums[mid] > nums[mid + 1]) {
                privot = mid;
                break;
            } else if (nums[mid] > nums[nums.size() - 1]) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }

        if (low < nums.size() - 1 && nums[low] > nums[low + 1]) privot = low;

        if (nums[privot + 1] <= target && target <= nums.back()) {
            low = privot + 1;
            high = nums.size() - 1;
        } else {
            low = 0;
            high = privot;
        }

        while (low < high) {
            int mid = low + (high - low) / 2;
            if (nums[mid] == target) return mid;
            if (nums[mid] < target) low = mid + 1;
            else high = mid - 1;
        }

        return nums[low] == target ? low : -1;
    }

    static int bsearch2(const vector<int>& nums, int target) {
        auto low = 0, high = (int) nums.size() - 1;

        while (low < high) {
            int mid = low + (high - low) / 2;
            if (nums[mid] > nums[high]) {
                low = mid + 1;
            } else {
                high = mid;
            }
        }

        if (target > nums.back()) {
            high = low ? low - 1 : nums.size() - 1;
            low = 0;
        } else {
            high = nums.size() - 1;
        }

        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (nums[mid] == target) return mid;
            if (nums[mid] < target) low = mid + 1;
            else high = mid - 1;
        }

        return -1;
    }
};

int main()
{
    cout << Solution::bsearch2({2,3,4,5,6,7,1}, 1) << endl;
    cout << Solution::bsearch2({3,4,5,1,2}, 2) << endl;
    cout << Solution::bsearch2({3,4,5,6,7,1,2}, 3) << endl;
    cout << Solution::bsearch2({7,1,2,3,4,5,6}, 4) << endl;
    cout << Solution::bsearch2({1,2}, 4) << endl;
    cout << Solution::bsearch2({1}, 1) << endl;
    cout << Solution::bsearch2({4,5,1,2,3}, 1) << endl;
}
