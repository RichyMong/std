#include <iostream>
#include <vector>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int findKthLargest(vector<int>& nums, int k) {
        int r = k;
        while (r > 0) {
            int s = 0;
            int end = nums.size() + r - k - 1;
            int m = (end - s) / 2;
            while (m >= s) {
                auto root = m;
                auto dest = root;
                while (root * 2 + 1 <= end) {
                    auto left = root * 2 + 1;
                    auto right = left + 1;
                    if (nums[dest] < nums[left]) {
                        dest = left;
                    }
                    if (right <= end && nums[dest] < nums[right]) {
                        dest = right;
                    }
                    if (dest == root) break;
                    swap(nums[root], nums[dest]);
                    root = dest;
                }
                --m;
            }
            swap(nums[0], nums[end]);
            --r;
        }

        return nums[nums.size() - k];
    }

    static int findKthLargest2(vector<int>& nums, int k) {
        int r = k;
        int s = 0;
        while (r > 0) {
            int m = s + (nums.size() - s) / 2;
            while (m >= s) {
                auto root = m;
                auto left = s + (root - s) * 2 + 1;
                while (left < nums.size()) {
                    auto dest = root;
                    auto right = left + 1;
                    if (nums[dest] < nums[left]) {
                        dest = left;
                    }
                    if (right < nums.size() && nums[dest] < nums[right]) {
                        dest = right;
                    }
                    if (dest == root) break;
                    swap(nums[root], nums[dest]);
                    root = dest;
                    left = s + (root - s) * 2 + 1;
                }
                --m;
            }
            --r;
            ++s;
        }

        return nums[k - r - 1];
    }
};

int main()
{
    vector<int> v { 3, 2, 1, 5, 6, 4};
    cout << Solution::findKthLargest2(v, 2) << endl;
    pretty_print(v);
    cout << Solution::findKthLargest2(v, 3) << endl;
    pretty_print(v);
    cout << Solution::findKthLargest2(v, 4) << endl;
    pretty_print(v);
    cout << Solution::findKthLargest2(v, 1) << endl;
    pretty_print(v);
    cout << Solution::findKthLargest2(v, 6) << endl;
    pretty_print(v);
    cout << Solution::findKthLargest2(v, 5) << endl;
    pretty_print(v);
    vector<int> vi { -1, 2, 0 };
    cout << Solution::findKthLargest2(vi, 2) << endl;
    pretty_print(vi);
    vector<int> vi2 { -1, 2, 0, 3 };
    cout << Solution::findKthLargest2(vi2, 2) << endl;
    pretty_print(vi2);
}
