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
        for (auto it = wordDict.begin(); it != wordDict.end(); ++it) {
            string tmp = *it;
            for (auto tit = wordDict.begin(); tit != wordDict.end(); ++tit) {
                if (it != tit) {
                    tmp += *tit;
                }
                if (tmp == s) return true;
            }
        }

        return false;
    }

    static bool wordBreak2(const string& s, const unordered_set<string>& wordDict) {
        vector<string> keys(wordDict.begin(), wordDict.end());
        sort(keys.begin(), keys.end(), [](const string& a, const string& b) { return a.size() > b.size() ? -1 : (b.size() - a.size()); });

        string t = s;
        for (auto it = keys.begin(); it != keys.end(); ++it) {
            auto& tmp = *it;
            while (true) {
                auto pos = t.find(tmp);
                if (pos == string::npos) {
                    break;
                }
                t.erase(pos, tmp.size());
            }
        }

        return t.empty();
    }
};

int main()
{
    cout << Solution::wordBreak("leetcode", { "leet", "code"}) << endl;
    cout << Solution::wordBreak("leetcode", { "le", "code", "et"}) << endl;
    cout << Solution::wordBreak("leetcode", { "le", "ode", "et"}) << endl;
    cout << '\n';
    cout << Solution::wordBreak2("leetcode", { "leet", "code"}) << endl;
    cout << Solution::wordBreak2("leetcode", { "le", "code", "et"}) << endl;
    cout << Solution::wordBreak2("leetcode", { "le", "ode", "et"}) << endl;
    cout << Solution::wordBreak2("bb", { "a", "b", "bbb"}) << endl;
    cout << Solution::wordBreak2("dogs", { "dog", "s", "gs"}) << endl;
}
