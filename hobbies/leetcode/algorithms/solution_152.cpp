// Maximum Product Subarray -
// Find the contiguous subarray within an array
// (containing at least one number) which has the largest product.
//
// For
// example, given the array [2,3,-2,4],
// the contiguous subarray [2,3] has
// the largest product = 6.
//
// https://leetcode.com/problems/maximum-product-subarray/
#include "solution.h"

using namespace std;

class Solution {
public:
    static int maxProduct(const vector<int>& nums) {
        if (nums.empty()) return 0;

        auto max_product = nums[0], max_cur = nums[0], min_cur = nums[0];

        for (int i = 1; i < nums.size(); ++i) {
            auto tmp_max = max(max(max_cur * nums[i], min_cur * nums[i]), nums[i]);
            auto tmp_min = min(min(max_cur * nums[i], min_cur * nums[i]), nums[i]);
            max_cur = tmp_max;
            min_cur = tmp_min;
            max_product = max(max_product, max_cur);
        }

        return max_product;
    }

    static int maxProduct2(const vector<int>& nums) {
        if (nums.empty()) return 0;

        vector<vector<int>> result(nums.size(), vector<int>(nums.size()));

        auto max_product = nums[0];

        for (int i = 0; i < nums.size(); ++i) {
            result[i][i] = nums[i];
        }

        for (int i = 0; i < nums.size(); ++i) {
            for (int j = i + 1; j < nums.size(); ++j) {
                result[i][j] = result[i][j - 1] * nums[j];
                if (result[i][j] > max_product) {
                    max_product = result[i][j];
                }
            }
        }

        return max_product;
    }
};

int main()
{
    cout << Solution::maxProduct({ 2, 3, -2, 4, 5}) << endl;
    cout << Solution::maxProduct({ -3, 1, 4}) << endl;
    cout << Solution::maxProduct({ -2, 3, -4}) << endl;
    cout << Solution::maxProduct({ -4, -3, -2}) << endl;
    cout << Solution::maxProduct({ -4, -3, 0, 4, 5}) << endl;
}
