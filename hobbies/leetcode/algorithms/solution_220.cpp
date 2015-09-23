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
            auto e = us.lower_bound(nums[i] - t);
            if (e != us.end() && *e - nums[i] <= t)
                return true;
            us.insert(nums[i]);
        }

        return false;
    }
};

int main()
{
    cout << Solution::containsNearbyAlmostDuplicate({1, 2, 3, 2}, 3, 0) << endl;
    cout << Solution::containsNearbyAlmostDuplicate({1, 2, 3, 2}, 4, 0) << endl;
    cout << Solution::containsNearbyAlmostDuplicate({1, 2, 3, 4}, 2, 0) << endl;
    cout << Solution::containsNearbyAlmostDuplicate({1, 2, 3, 2}, 1, 2) << endl;
    cout << Solution::containsNearbyAlmostDuplicate({1, 3, 6, 2}, 1, 2) << endl;
}
