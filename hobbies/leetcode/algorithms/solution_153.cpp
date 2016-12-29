// find-minimum-in-rotated-sorted-array - Suppose a sorted array is rotated at
// some pivot unknown to you beforehand.
// (i.e., 0 1 2 4 5 6 7 might become 4 5 6 7 0 1 2).
// Find the minimum element.
// You may assume no duplicate exists in the array.
// https://leetcode.com/problems/find-minimum-in-rotated-sorted-array
#include "solution.h"

using namespace std;

class Solution {
public:
    int findMin(const vector<int>& nums) {
        auto size = nums.size();
        auto first = nums.begin();
        while (size > 1) {
            auto mid = (size >> 1);
            auto half = first + mid;
            if (*first < *half && *half < *(first + size - 1)) {
                break;
            } else if (*first < *half) {
                first = half + 1;
                size -= mid + 1;
            } else {
                ++first;
                size = mid;
            }
        }

        return *first;
    }
};

int main()
{
    Solution s;
    cout << s.findMin(vector<int> { 4, 5, 6, 7, 1, 2, 3 }) << endl;
    cout << s.findMin(vector<int> { 7, 1, 2, 3, 4, 5, 6 }) << endl;
    cout << s.findMin(vector<int> { 2, 3, 4, 5, 6, 7, 1 }) << endl;
    cout << s.findMin(vector<int> { 1, 2, 3, 4, 5, 6, 7 }) << endl;
    cout << s.findMin(vector<int> { 8, 9, 10, 1, 2, 3, 4, 5, 6, 7 }) << endl;
}
