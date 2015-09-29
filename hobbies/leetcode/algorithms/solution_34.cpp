// Search for a Range. Reference url is listed bellow:
// https://leetcode.com/problems/search-for-a-range/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static vector<int> searchRange(const vector<int>& nums, int target) {
        int start = 0, end = (int) nums.size() - 1;
        int mid = -1;
        while (start <= end) {
            mid = start + (end - start) / 2;
            if (nums[mid] == target) break;
            if (nums[mid] < target) start = mid + 1;
            else end = mid - 1;
        }

        if (start > end) {
            return vector<int> { -1, -1 };
        }

        start = mid - 1; end = mid + 1;
        while (start >= 0 && nums[start] == target)
            --start;
        while (end < (int)nums.size() && nums[end] == target)
            ++end;
        return vector<int> { start + 1, end - 1 };
    }

    static vector<int> searchRange2(const vector<int>& nums, int target) {
        vector<int> result { -1, -1 };

        int start = 0, end = (int) nums.size() - 1;
        while (start < end) {
            int mid = start + (end - start) / 2;
            if (nums[mid] < target) start = mid + 1;
            else end = mid;
        }

        if (nums[start] != target) {
            return result;
        } else {
            result[0] = start;
        }

        end = (int)nums.size() - 1;
        while (start < end) {
            int mid = start + (end - start) / 2 + 1;
            if (nums[mid] > target) end = mid - 1;
            else start = mid;
        }

        result[1] = end;

        return result;
    }
};

int main()
{
    vector<int> nums {5, 7, 7, 8, 8, 10};

    auto v = Solution::searchRange(nums, 8);
    pretty_print(v);
    v = Solution::searchRange(nums, 7);
    pretty_print(v);
    v = Solution::searchRange(nums, 5);
    pretty_print(v);
    v = Solution::searchRange(nums, 10);
    pretty_print(v);
    v = Solution::searchRange(nums, 11);
    pretty_print(v);
    v = Solution::searchRange({1, 1, 1, 1, 1, 1, 1}, 1);
    pretty_print(v);
    cout << endl;
    v = Solution::searchRange2(nums, 8);
    pretty_print(v);
    v = Solution::searchRange2(nums, 7);
    pretty_print(v);
    v = Solution::searchRange2(nums, 5);
    pretty_print(v);
    v = Solution::searchRange2(nums, 10);
    pretty_print(v);
    v = Solution::searchRange2(nums, 11);
    pretty_print(v);
    v = Solution::searchRange2({1, 1, 1, 1, 1, 1, 1}, 1);
    pretty_print(v);
}
