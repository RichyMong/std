// third-maximum-number - Given a non-empty array of integers, return the third
// maximum number in this array. If it does not exist, return the maximum number.
// The time complexity must be in O(n).
// Example 1:
// Input: [3, 2, 1]
// Output: 1
//
// Explanation: The third maximum is 1.
//
// Example 2:
// Input: [1, 2]
// Output: 2
//
// Explanation: The third maximum does not exist, so the maximum (2) is returned instead.
//
// Example 3:
// Input: [2, 2, 3, 1]
// Output: 1
//
// Explanation: Note
// that the third maximum here means the third maximum distinct number.
// Both numbers with value 2 are both considered as second maximum.
// https://leetcode.com/problems/third-maximum-number
#include "solution.h"
#include <string.h>

using namespace std;

class Solution {
public:
    int thirdMax(const vector<int>& nums) {
        set<int> res;
        for (size_t i = 0; i < nums.size(); ++i) {
            if (res.find(nums[i]) == res.end()) {
                if (res.size() < 3) {
                    res.insert(nums[i]);
                } else if (*res.begin() < nums[i]) {
                    res.erase(res.begin());
                    res.insert(nums[i]);
                }
            }
        }

        return res.size() < 3 ? *res.rbegin() : *res.begin();
    }

    int thirdMax2(const vector<int>& nums) {
        int res[3];
        int n = 0;
        for (size_t i = 0; i < nums.size(); ++i) {
            if ((n >= 1 && nums[i] == res[0]) ||
                (n >= 2 && nums[i] == res[1]) ||
                (n >= 3 && nums[i] == res[2])) {
                 continue;
            }

            int index = n;

            if (n == 0 || nums[i] > res[0]) {
                index = 0;
            } else if (n > 1 && nums[i] > res[1]) {
                index = 1;
            } else if (n > 2 && nums[i] > res[2]) {
                index = 2;
            }

            if (index < 3) {
                if (index < 2) {
                    memmove(res + index + 1, res + index, min(n, 2 - index) * sizeof(int));
                }
                res[index] = nums[i];
                if (n < 3) n += 1;
            }
        }

        return n < 3 ? res[0] : res[2];
    }
};

int main()
{
    Solution s;
    cout << s.thirdMax2(vector<int> { 3, 2, 1 }) << endl;
    cout << s.thirdMax2(vector<int> { 1, 2 }) << endl;
    cout << s.thirdMax2(vector<int> { 2, 2, 3, 1 }) << endl;
    cout << s.thirdMax2(vector<int> { 5, 2, 2 }) << endl;
    cout << s.thirdMax2(vector<int> { 1, 2, 2, 5, 3, 5 }) << endl;
}
