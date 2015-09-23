#include "solution.h"

using namespace std;

class Solution {
public:
    static void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
        int i = 0, j = 0;

        while (j < n) {
            while (i < m && nums1[i] <= nums2[j]) ++i;

            if (i == m) break;

            auto old = j;
            while (j < n && nums2[j] < nums1[i]) ++j;

            nums1.insert(nums1.begin() + i, nums2.begin() + old, nums2.begin() + j);
            m += j - old;
        }

        if (j != n) {
            nums1.insert(nums1.begin() + i, nums2.begin() + j, nums2.end());
        }
    }

    static void merge2(vector<int>& nums1, int m, vector<int>& nums2, int n) {
        int i = m - 1, j = n - 1, end = m + n - 1;
        while (j >= 0) {
            nums1[end--] = (i >= 0 && nums1[i] > nums2[j]) ? nums1[i--] : nums2[j--];
        }
    }
};

int main()
{
    vector<int> v1 { 0 };
    vector<int> v2 { 1 };
    Solution::merge(v1, 0, v2, 1);
    pretty_print(v1);
}
