// Intersection of Two Arrays II - Given two arrays, write a function to compute
// their intersection.
// Example:
// Given nums1 = [1, 2, 2, 1], nums2 = [2,
// 2], return [2, 2].
// Note:
// Each element in the result should appear as
// many times as it shows in both arrays.
// The result can be in any order.
//
//
// Follow up:
// What if the given array is already sorted? How would you
// optimize your algorithm?
// What if nums1's size is small compared to nums2
// s size? Which algorithm is better?
// What if elements of nums2 are stored
// on disk, and the memory is limited such that you cannot load all elements
// into the memory at once?
// https://leetcode.com/problems/intersection-of-two-arrays-ii/
#include "solution.h"

using namespace std;

class Solution {
public:
    vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {
        sort(nums1.begin(), nums1.end());
        sort(nums2.begin(), nums2.end());
        vector<int> res;
        set_intersection(nums1.begin(), nums1.end(), nums2.begin(), nums2.end(), back_inserter(res));

        return res;
    }

};

int main()
{
    vector<int> v1 { 1, 2, 2, 1 }, v2 { 2, 2 };
    pretty_print(Solution().intersection(v1, v2));
}
