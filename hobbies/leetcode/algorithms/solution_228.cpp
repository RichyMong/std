#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Solution {
public:
    static vector<string> summaryRanges(const vector<int>& nums) {
        if (nums.empty()) {
            return vector<string> {};
        }

        vector<string> result;

        int j = 0;
        for (int i = 1; i < nums.size(); ++i) {
            if (nums[i] != nums[i - 1] + 1) {
                if (j == i - 1) {
                    result.push_back(to_string(nums[j]));
                } else {
                    result.push_back(to_string(nums[j]) + "->" + to_string(nums[i - 1]));
                }
                j = i;
            }
        }

        if (j + 1 == nums.size()) {
            result.push_back(to_string(nums[j]));
        } else {
            result.push_back(to_string(nums[j]) + "->" + to_string(nums[nums.size() - 1]));
        }

        return result;
    }
};

int main()
{
    auto x = Solution::summaryRanges({0, 1, 2, 4, 5, 6, 7});
    for (auto s : x) {
        cout << s << ' ';
    }
    cout << endl;
}
