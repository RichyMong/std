#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int majorityElement(const vector<int>& nums) {
        int target = nums[0];
        int count = 1;

        for (int i = 1; i < nums.size(); ++i) {
            count += (nums[i] == target) ? 1 : -1;
            if (count < 0) {
                target = nums[i];
                count = 1;
            }
        }

        return target;
    }

};

int main()
{
    cout << Solution::majorityElement({1, 2, 2, 3}) << endl;
    cout << Solution::majorityElement({1, 2, 3, 1, 1}) << endl;
}
