#include "solution.h"
#include <vector>
#include <algorithm>

class Solution {
public:
    using type = std::vector<int>;

    static std::vector<type> permute(const type& nums)
    {
        if (nums.empty()) {
            return std::vector<type> {{}};
        }

        std::vector<type> result;
        for (type::size_type i = 0; i != nums.size(); ++i) {
            type rest;
            std::copy(nums.begin(), nums.begin() + i, std::back_inserter(rest));
            std::copy(nums.begin() + i + 1, nums.end(), std::back_inserter(rest));
            auto sub = permute(rest);
            for (auto& x : sub) {
                x.push_back(nums[i]);
                result.push_back(x);
            }
        }
        return result;
    }
};

int main()
{
    std::vector<int> nums { 1, 2, 3 };
    auto ret = Solution::permute(nums);
    for (auto& x : ret) {
        pretty_print(x.begin(), x.end());
    }
}
