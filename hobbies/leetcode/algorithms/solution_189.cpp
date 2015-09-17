#include <iostream>
#include <vector>
#include "solution.h"

using namespace std;

class Solution {
public:
    static void reverse(vector<int>& nums, int i, int j) {
        while (i < j) {
            swap(nums[i++], nums[j--]);
        }
    }

    static void rotate(vector<int>& nums, int k) {
        auto n = nums.size();
        if (n == 0) return;

        k %= n;
        if (k == 0) return;

        reverse(nums, 0, n - k - 1);
        reverse(nums, n - k, n - 1);
        reverse(nums, 0, n - 1);
    }

    static int gcd(int m, int n) {
        while (m != 0) {
            if (n >=m ) {
                n -= m;
            } else {
                swap(m, n);
            }
        }
        return n;
    }

    static void rotate2(vector<int>& nums, int k) {
        auto n = nums.size();
        if (n == 0) return;

        k = k % n;
        auto cycles = gcd(k, n);
        if (cycles == n) return;

        int i = 0, j = 0, r = k, t;
        while (i < n) {
            r = (r >= n ? r - n : r);
            t = nums[r];
            nums[r] = nums[j];
            nums[j] = t;
            i++;
            r += k;
            if (i % (n / cycles) == 0) {
                r++;
                j++;
            }
        }
    }

};

int main()
{
    {
        vector<int> nums { 1,2,3,4,5,6,7 };

        Solution::rotate(nums, 3);
        pretty_print(nums);
    }

    {
        vector<int> nums { 1, 2 };

        Solution::rotate2(nums, 1);
        pretty_print(nums);
    }
}
