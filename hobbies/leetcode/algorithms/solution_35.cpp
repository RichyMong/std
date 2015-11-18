// Search Insert Position - Given a sorted array and a target value, return the
// index if the target is found. If not, return the index where it would be if
// it were inserted in order.
// You may assume no duplicates in the array.
//
// Here are few examples.
// [1,3,5,6], 5  2
// [1,3,5,6], 2  1
// [1,3,5,6], 7  4
// [1,3,5,6], 0  0
//
// https://leetcode.com/problems/search-insert-position/
#include "solution.h"

using namespace std;

class Solution {
public:
    static int searchInsert(const vector<int>& nums, int target) {
        int low = 0, high = (int) nums.size() - 1;

        while (low <= high) {
            auto mid = (high - low) / 2 + low;
            if (nums[mid] == target) {
                return mid;
            } else if (nums[mid] > target) {
                if (mid == 0 || nums[mid - 1] < target) {
                    return mid;
                } else {
                    high = mid - 1;
                }
            } else {
                low = mid + 1;
            }
        }

        return nums.size();
    }

    static int searchInsert2(const vector<int>& nums, int target) {
        int low = 0, high = nums.size();

        while (low < high) {
            auto mid = (high - low) / 2 + low;
            if (nums[mid] < target) {
                low = mid + 1;
            } else {
                high = mid;
            }
        }

        return low;
    }
};

int main()
{
    vector<int> nums { 1, 3, 5, 6 };

    cout << Solution::searchInsert(nums, 5) << endl;
    cout << Solution::searchInsert(nums, 2) << endl;
    cout << Solution::searchInsert(nums, 7) << endl;
    cout << Solution::searchInsert(nums, 0) << endl;
    cout << Solution::searchInsert(nums, 1) << endl;
}
