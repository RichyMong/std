#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static bool compare_func(int x, int y) {
        string sx = to_string(x);
        string sy = to_string(y);

        auto i = 0;

        for (i = 0; i < sx.size() && i < sy.size(); ++i) {
            if (sx[i] != sy[i]) {
                return sx[i] > sy[i];
            }
        }

        if (sx.size() < sy.size())
            return sy[i] > sx[i - 1];
        else if (sx.size() > sy.size())
            return sx[i] > sy[i - 1];
        return true;
    }

    static string largestNumber(vector<int>& nums) {
        vector<string> numstrs;
        for (auto x : nums) {
            numstrs.push_back(to_string(x));
        }
        sort(numstrs.begin(), numstrs.end(), [](const string& x, const string& y) {
                    return x + y > y + x;
                });
        if (numstrs[0] == "0") {
            return "0";
        }

        return accumulate(numstrs.begin(), numstrs.end(), string{});
    }

    static string largestNumber2(vector<int>& nums) {
        sort(nums.begin(), nums.end(), Solution::compare_func);
        if (nums[0] == 0) {
            return "0";
        }
        ostringstream os;
        for (auto x : nums) {
            os << x;
        }
        return os.str();
    }
};

int main()
{
    vector<int> nums { 3, 30, 34, 5, 9 };
    std::cout << Solution::largestNumber(nums) << std::endl;
    std::cout << Solution::largestNumber2(nums) << std::endl;
    pretty_print(nums);
    vector<int> zeros { 0, 0, 0, 0, 0, 0 };
    std::cout << Solution::largestNumber(zeros) << std::endl;
    std::cout << Solution::largestNumber(zeros) << std::endl;
}
