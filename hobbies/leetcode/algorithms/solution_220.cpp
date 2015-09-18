#include <iostream>
#include <vector>
#include <map>
#include <set>

using namespace std;

class Solution {
public:
    static bool containsNearbyAlmostDuplicate(const vector<int>& nums, int k, int t) {
        if (k < 0 || t < 0) return false;

        set<int> us;

        for (int i = 0; i != nums.size(); ++i) {
            if (i > k) us.erase(nums[i - k - 1]);
            for (auto e = us.begin(); e != us.end(); ++e) {
                if (abs(*e - nums[i]) <= t)
                    return true;
            }
            us.insert(nums[i]);
        }

        return false;
    }
};

int main()
{
    cout << Solution::containsNearbyAlmostDuplicate({1, 2, 3, 2}, 3, 0) << endl;
    cout << Solution::containsNearbyAlmostDuplicate({1, 2, 3, 2}, 4, 0) << endl;
    cout << Solution::containsNearbyAlmostDuplicate({1, 2, 3, 2}, 2, 0) << endl;
    cout << Solution::containsNearbyAlmostDuplicate({1, 2, 3, 2}, 1, 2) << endl;
}
