#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

class Solution {
public:
    static bool containsDuplicate(vector<int>& nums) {
        return unordered_set<int>(nums.begin(), nums.end()).size() != nums.size();
    }
};

int main()
{
    vector<int> nums { 1, 2, 3, 4, 5 };
    cout << Solution::containsDuplicate(nums) << endl;
    nums[1] = 1;
    cout << Solution::containsDuplicate(nums) << endl;
}
