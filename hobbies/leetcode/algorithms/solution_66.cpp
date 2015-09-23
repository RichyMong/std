#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static vector<int> plusOne(const vector<int>& digits) {
        if (digits.empty()) return vector<int> {};

        auto result = digits;
        int flag = 1;

        for (int i = result.size() - 1; i >= 0; --i) {
            result[i] += flag;
            if (result[i] < 10) {
                flag = 0;
                break;
            }

            result[i] %= 10;
        }

        if (flag) result.insert(result.begin(), 1);

        return result;
    }

    static vector<int> plusOne2(vector<int>& digits) {
        for (int i = digits.size(); i--; digits[i] = 0) {
            if (digits[i]++ < 9)
                return digits;
        }

        ++digits[0];
        digits.push_back(0);

        return digits;
    }
};

int main()
{
    vector<int> vi { 1, 2, 3 };
    auto r = Solution::plusOne2(vi);
    pretty_print(r);
    r = Solution::plusOne({9, 9, 9});
    pretty_print(r);
}
