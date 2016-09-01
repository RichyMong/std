// Intersection of Two Arrays - Given two arrays, write a function to compute
// their intersection.
// Example:
// Given nums1 = [1, 2, 2, 1], nums2 = [2,
// 2], return [2].
// Note:
// Each element in the result mst be unique.
//
// The result can be in any order.
// https://leetcode.com/problems/intersection-of-two-arrays/
#include "solution.h"

using namespace std;

class Solution {
public:
    vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {
        set<int> s1(nums1.begin(), nums1.end()), s2(nums2.begin(), nums2.end());
        vector<int> res;
        set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), back_inserter(res));

        return res;
    }

};

int main()
{
    vector<int> v1 { 1, 2, 2, 1 }, v2 { 2, 2 };
    pretty_print(Solution().intersection(v1, v2));
}
