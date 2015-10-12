// Find Peak Element A peak element is an element that is greater than its neighbors.
// Given an input array where num[i] != num[i+1], find a peak element and return its index.
// The array may contain multiple peaks, in that case return the index to any one of the peaks is fine.
// You may imagine that num[-1] = num[n] = -.
// For example, in array [1, 2, 3, 1], 3 is a peak element and your function should return the index number 2.
// click to show spoilers.
// Note:
// Your solution should be in logarithmic complexity.
// Credits:Special thanks to @ts for adding this problem and creating all test cases.
// https://leetcode.com/problems/find-peak-element/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int findPeakElement(const vector<int>& nums) {
        int start = 0, end = nums.size() - 1;
        int last = (int) nums.size() - 1;

        while (start < end) {
            int mid = start + (end - start) / 2;

            if (mid == 0) {
                if (nums[mid] > nums[mid + 1]) return mid;
                start = mid + 1;
            } else if (mid == last) {
                if (nums[mid] > nums[mid - 1]) return mid;
                end = mid - 1;
            } else if (nums[mid] > nums[mid + 1] && nums[mid] > nums[mid - 1]) {
                return mid;
            } else if (nums[mid] < nums[mid + 1]) {
                start = mid + 1;
            } else {
                end = mid - 1;
            }
        }

        return start;
    }

    static int findPeakElement2(const vector<int>& nums) {
        int start = 0, end = nums.size() - 1;

        while (start < end - 1) {
            int mid = start + (end - start) / 2;

            if (nums[mid] > nums[mid + 1] && nums[mid] > nums[mid - 1]) {
                return mid;
            } else if (nums[mid] < nums[mid + 1]) {
                start = mid + 1;
            } else {
                end = mid - 1;
            }
        }

        return nums[start] > nums[end] ? start : end;
    }
};

int main()
{
    cout << Solution::findPeakElement({1, 2, 3, 1}) << endl;
    cout << Solution::findPeakElement({1}) << endl;
    cout << Solution::findPeakElement({1, 2}) << endl;
    cout << Solution::findPeakElement({2, 1}) << endl;
    cout << Solution::findPeakElement({-2147483648}) << endl;
    cout << Solution::findPeakElement({1, 3, 2, 1}) << endl;
    cout << '\n';
    cout << Solution::findPeakElement2({1, 2, 3, 1}) << endl;
    cout << Solution::findPeakElement2({1}) << endl;
    cout << Solution::findPeakElement2({1, 2}) << endl;
    cout << Solution::findPeakElement2({2, 1}) << endl;
    cout << Solution::findPeakElement2({-2147483648}) << endl;
    cout << Solution::findPeakElement2({1, 3, 2, 1}) << endl;
}
