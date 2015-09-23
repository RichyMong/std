#include <iostream>
#include <vector>
#include "solution.h"

using namespace std;

class Solution {
public:
    static vector<int> productExceptSelf(const vector<int>& nums) {
        if (nums.empty()) return vector<int> {};

        vector<int> asscend(nums.size(), 1);
        for (auto i = 1; i < nums.size(); ++i) {
            asscend[i] = asscend[i - 1] * nums[i - 1];
        }

        vector<int> descend(nums.size(), 1);
        for (int i = nums.size() - 2; i >= 0; --i) {
            descend[i] = descend[i + 1] * nums[i + 1];
        }

        vector<int> result;
        result.reserve(nums.size());
        for (auto i = 0; i < nums.size(); ++i) {
            result.push_back(asscend[i] * descend[i]);
        }

        return result;
    }

    static vector<int> productExceptSelf2(const vector<int>& nums) {
        vector<int> result(nums.size(), 1);

        for (auto i = 1; i < nums.size(); ++i) {
            result[i] = result[i - 1] * nums[i - 1];
        }

        int product = 1;
        for (int i = (int) nums.size() - 1; i >= 0; --i) {
            result[i] *= product;
            product *= nums[i];
        }

        return result;
    }

    static vector<int> productExceptSelf3(const vector<int>& nums) {
        vector<int> result(nums.size(), 1);

        int ltor = 1, rtol = 1;
        for (auto i = 0; i < nums.size(); ++i) {
            result[i] *= ltor;
            ltor *= nums[i];
            result[nums.size() - i - 1] *= rtol;
            rtol *= nums[nums.size() - 1 - i];
        }

        return result;
    }

};

int main()
{
    auto ret = Solution::productExceptSelf3({1, 2, 3, 4 });
    pretty_print(ret);
}
