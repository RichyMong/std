#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int removeDuplicates(vector<int>& nums) {
        int last;

        int end = nums.size();
        for (int i = 0; i < end; ) {
            if (i == 0) last = nums[i++];
            else if (last != nums[i]) last = nums[i++];
            else {
                for (int j = i + 1; j < end; ++j) {
                    nums[j - 1] = nums[j];
                }
                --end;
            }
        }

        return end;
    }

    static int removeDuplicates2(vector<int>& nums) {
        int pos = 0;

        for (int i = 0; i < nums.size(); ++i) {
            if (i == 0 || nums[i] != nums[pos - 1])
                nums[pos++] = nums[i];
        }

        return pos;
    }

};

int main()
{
    auto v = vector<int> { 1, 1, 2, 2, 3, 4 };
    cout << Solution::removeDuplicates2(v) << endl;
    pretty_print(v);

    v = vector<int> { 1, 1 };
    cout << Solution::removeDuplicates2(v) << endl;
    pretty_print(v);

    v = vector<int> { 1, 2 };
    cout << Solution::removeDuplicates2(v) << endl;
    pretty_print(v);
}
