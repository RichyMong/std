#include <vector>
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int removeElement(vector<int>& nums, int val) {
        if (nums.empty())
            return 0;

        int last = nums.size() - 1;
        for (int i = 0; i <= last; ) {
            if (nums[i] == val) {
                swap(nums[i], nums[last--]);
            } else {
                ++i;
            }
        }

        return last + 1;
    }
};

int main()
{
    vector<int> v1 { 1, 2, 3, 4, 5 };
    vector<int> v2 { 1, 2, 1, 4, 1 };
    vector<int> v3 { 1, 1, 1, 1, 1 };

    cout << Solution::removeElement(v1, 6) << endl;
    pretty_print(v1.begin(), v1.end());
    cout << Solution::removeElement(v2, 1) << endl;
    pretty_print(v2.begin(), v2.end());
    cout << Solution::removeElement(v3, 1) << endl;
    pretty_print(v3.begin(), v3.end());

    return 0;
}
