#include <iostream>
#include <algorithm>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int rob(const vector<int>& nums) {
        if (nums.size() == 1) return nums[0];

        int max_prev_1 = 0, max_prev_2 = 0;

        for (int i = 0; i < (int) nums.size() - 1; ++i) {
            auto temp = max(max_prev_2 + nums[i], max_prev_1);
            max_prev_2 = max_prev_1;
            max_prev_1 = temp;
        }

        int max_prev_t1 = 0, max_prev_t2 = 0;
        for (int i = 1; i < (int) nums.size(); ++i) {
            auto temp = max(max_prev_t2 + nums[i], max_prev_t1);
            max_prev_t2 = max_prev_t1;
            max_prev_t1 = temp;
        }

        return max(max_prev_1, max_prev_t1);
    }

};

int main()
{
    cout << Solution::rob({10, 2, 6, 2, 4, 24, 7}) << endl;
    cout << Solution::rob({2, 6, 2, 4, 24, 7}) << endl;
    cout << Solution::rob({4, 12, 6}) << endl;
    cout << Solution::rob({15, 4, 6}) << endl;
    cout << Solution::rob({5, 6}) << endl;
    cout << Solution::rob({6, 5}) << endl;
    cout << Solution::rob({5}) << endl;
}
