#include <iostream>
#include <algorithm>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int rob(const vector<int>& nums) {
        const int n = nums.size();

        vector<int> maxn(n);
        for (int i = 0; i < n; ++i) {
            if (i >= 3) {
                maxn[i] = max(maxn[i - 1], max(nums[i] + maxn[i - 2], nums[i] + maxn[i - 3]));
            } else if (i >= 2) {
                maxn[i] = max(maxn[i - 1], nums[i] + maxn[i - 2]);
            } else if (i >= 1) {
                maxn[i] = max(maxn[i - 1], nums[i]);
            } else {
                maxn[i] = nums[i];
            }
        }

        return n > 0 ? maxn[n - 1] : 0;
    }

    static int rob2(const vector<int>& nums) {
        int max_prev_1 = 0, max_prev_2 = 0;

        for (int i = 0; i < nums.size(); ++i) {
            auto temp = max(max_prev_2 + nums[i], max_prev_1);
            max_prev_2 = max_prev_1;
            max_prev_1 = temp;
        }

        return max_prev_1;
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
    cout << "the second solution\n";
    cout << Solution::rob2({10, 2, 6, 2, 4, 24, 7}) << endl;
    cout << Solution::rob2({2, 6, 2, 4, 24, 7}) << endl;
    cout << Solution::rob2({4, 12, 6}) << endl;
    cout << Solution::rob2({15, 4, 6}) << endl;
    cout << Solution::rob2({5, 6}) << endl;
    cout << Solution::rob2({6, 5}) << endl;
    cout << Solution::rob2({5}) << endl;
}
