#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class Solution {
public:
    static bool containsNearbyDuplicate(const vector<int>& nums, int k) {
        unordered_map<int, int> um;

        for (int i = 0; i < nums.size(); ++i) {
            auto e = um.find(nums[i]);
            if (e != um.end() && e->second + k >= i) {
                return true;
            } else {
                um[nums[i]] = i;
            }
        }

        return false;
    }

    static bool containsNearbyDuplicate2(const vector<int>& nums, int k) {
        unordered_set<int> us;

        for (int i = 0; i < nums.size(); ++i) {
            if (i > k) us.erase(nums[i - k - 1]);
            if (!us.insert(nums[i]).second) return true;
        }

        return false;
    }
};

int main()
{
    cout << Solution::containsNearbyDuplicate({1, 2, 3, 2}, 3) << endl;
    cout << Solution::containsNearbyDuplicate({1, 2, 3, 2}, 4) << endl;
    cout << Solution::containsNearbyDuplicate({1, 2, 3, 2}, 2) << endl;
    cout << Solution::containsNearbyDuplicate({1, 2, 3, 2}, 1) << endl;
    cout << Solution::containsNearbyDuplicate2({1, 2, 3, 2}, 3) << endl;
    cout << Solution::containsNearbyDuplicate2({1, 2, 3, 2}, 4) << endl;
    cout << Solution::containsNearbyDuplicate2({1, 2, 3, 2}, 2) << endl;
    cout << Solution::containsNearbyDuplicate2({1, 2, 3, 2}, 1) << endl;
}
