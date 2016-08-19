// Coin Change - You are given coins of different denominations and a total amount
// of money amount. Write a function to compute the fewest number of coins that
// you need to make up that amount. If that amount of money cannot be made up
// by any combination of the coins, return -1.
//
// Example 1:
// coins = [ 1, 2, 5], amount = 11
// return 3 (11 = 5 + 5 + 1)
//
// Example 2:
//
// coins = [2], amount = 3
// return -1.
//
// Note:
// You may assume that you have an infinite number of each kind of coin.
// Credits:Special thanks // to @jianchao.li.fighter for adding this problem
// and creating all test cases.
// https://leetcode.com/problems/coin-change/
#include "solution.h"
#include <stack>
#include <cstdio>

using namespace std;

class Solution {
public:
    static int coinChange(vector<int>::iterator beg, vector<int>::iterator end, int v) {
        if (v == 0) return 0;
        if (beg == end) return -1;

        auto it = lower_bound(beg, end, v);
        if (it != end && *it == v) {
            return 1;
        } else if (it == beg){
            return -1;
        } else {
            --it;
        }

        auto k = v / *it;
        auto x = v % *it;
        auto mink = v + 1;
        auto n = coinChange(beg, end, x);
        if (n >= 0) {
            mink = n + k;
        }
        for (int i = 1; i < k; ++i) {
            auto v = x + i * *it;
            auto n = coinChange(beg, it, v);
            if (n >= 0) {
                auto temp = n + k - i;
                if (temp < mink) mink = temp;
            }
        }
        return mink > v ? -1 : mink;
    }

    static int coinChangeDP(vector<int>& coins, int amount) {
        vector<int> steps(amount + 1, amount + 1);
        steps[0] = 0;
        for (int i = 1; i < amount + 1; ++i) {
            for (int j = 0; j < (int) coins.size(); ++j) {
                if (coins[j] <= i) {
                    steps[i] = min(steps[i - coins[j]] + 1, steps[i]);
                }
            }
        }
        return steps[amount] > amount ? -1 : steps[amount];
    }

    static int coinChange(vector<int>& coins, int amount) {
        sort(coins.begin(), coins.end());

        return coinChange(coins.begin(), coins.end(), amount);
    }
};

int main()
{
    vector<int> vi { 186,419,83,408 };
    vector<int> vii { 500,200,400};
    vector<int> v3 { 9, 8, 1 };
    // cout << Solution::coinChange(vi, 6249) << endl;
    // cout << Solution::coinChange(vii, 1600) << endl;
    // cout << Solution::coinChange(vii, 10) << endl;
    cout << Solution::coinChange(v3, 32) << endl;
    cout << Solution::coinChangeDP(vi, 6249) << endl;
    cout << Solution::coinChangeDP(vii, 1600) << endl;
    cout << Solution::coinChangeDP(vii, 10) << endl;
    cout << Solution::coinChangeDP(v3, 32) << endl;
    return 0;
}
