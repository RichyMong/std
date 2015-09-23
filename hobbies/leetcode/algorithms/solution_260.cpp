#include <iostream>
#include <vector>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include "solution.h"

using namespace std;

class Solution {
public:
    static vector<int> singleNumber(vector<int>& nums) {
        unordered_set<int> us;

        for (auto x : nums) {
            if (us.find(x) != us.end()) us.erase(x);
            else us.insert(x);
        }

        return vector<int>(us.begin(), us.end());
    }

    static vector<int> singleNumber2(vector<int>& nums) {
        auto ret = accumulate(nums.begin(), nums.end(), 0, bit_xor<int>());

        vector<int> result(2, 0);
        for (auto x : nums) {
            if (x & ret & -ret) {
                result[0] ^= x;
            } else {
                result[1] ^= x;
            }
        }

        return result;
    }

};

int main()
{
    vector<int> v { 1, 2, 3, 2, 1, 5 };
    auto y = Solution::singleNumber(v);
    pretty_print(y);
    auto z = Solution::singleNumber2(v);
    pretty_print(z);
}
