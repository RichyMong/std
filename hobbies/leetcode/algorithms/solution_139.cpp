// Word Break
// Given a string s and a dictionary of words dict, determine if s can be
// segmented into a space-separated sequence of one or more dictionary words.
// For example, given
// s = "leetcode",
// dict = ["leet", "code"].
//
// Return true because "leetcode" can be segmented as "leet code".
//
// https://leetcode.com/problems/word-break/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static bool wordBreak(const string& s, const unordered_set<string>& wordDict) {
        if (wordDict.find(s) != wordDict.end()) return true;

        if (s.empty()) return false;

        vector<vector<bool>> records(s.size(), vector<bool>(s.size(), false));

        for (int len = 1; len < s.size(); ++len) {
            for (int i = 0; i <= s.size() - len; ++i) {
                records[len - 1][i] = (wordDict.find(s.substr(i, len)) != wordDict.end());
            }
        }

        return false;
    }
};

int main()
{
    cout << Solution::wordBreak("leetcode", { "leet", "code"}) << endl;
    cout << Solution::wordBreak("leetcode", { "le", "code", "et"}) << endl;
    cout << Solution::wordBreak("leetcode", { "le", "ode", "et"}) << endl;
    cout << Solution::wordBreak("bb", { "a", "b", "bbb"}) << endl;
    cout << Solution::wordBreak("dogs", { "dog", "s", "gs"}) << endl;
}
