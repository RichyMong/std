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
            if (*first < *half) {
                if (mid == size - 1 || *half < *(first + size - 1))
                    break;
                else {
                    first = half + 1;
                    size -= mid + 1;
                }
            } else {
                ++first;
                size = mid;
            }
        }

        return *first;
    }
};

int main(int argc, char* argv[])
{
    Solution s;
    vector<int> v;
    for (int i = 1; i < (argc > 1 ? stoi(argv[1]): 10); i++) {
        v.push_back(i * 2 + 1);
    }
    for (size_t i = 0; i < v.size(); i++) {
        std::next_permutation(v.begin(), v.end());
        cout << s.findMin(v) << endl;
    }
    cout << s.findMin(vector<int>{1}) << endl;
    cout << s.findMin(vector<int>{3, 5}) << endl;
    cout << s.findMin(vector<int>{5, 3}) << endl;
    cout << s.findMin(vector<int>{5, 2, 3}) << endl;
    return 0;
}
