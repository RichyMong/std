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
    static bool helper(const unordered_set<string>& records, const string& s) {
        for (int len = 1; len < s.size(); ++len) {
            if  (records.find(s.substr(0, len)) != records.end() &&
                 records.find(s.substr(len, s.size() - len)) != records.end())
                return true;
        }

        return false;
    }

    static bool wordBreak(const string& s, const unordered_set<string>& wordDict) {
        const int size = s.size();

        if (size == 0) return false;

        unordered_set<string> records;

        for (int i = 0; i < size; ++i) {
            string tmp = s.substr(i, 1);
            if (wordDict.find(tmp) != wordDict.end())
                records.insert(tmp);
        }

        for (int len = 2; len <= size; ++len) {
            for (int j = 0; j <= size - len; ++j) {
                string tmp = s.substr(j, len);
                if (helper(records, tmp) || wordDict.find(tmp) != wordDict.end()) {
                    records.insert(tmp);
                }
            }
        }

        return records.find(s) != records.end();
    }

    static bool wordBreak2(const string& s, const unordered_set<string>& wordDict) {
        vector<bool> records(s.size() + 1, false);

        records[0] = true;

        for (int len = 1; len <= s.size(); ++len) {
            for (int i = len - 1; i >= 0;  --i) {
                if (records[i] && !records[len]) {
                    auto word = s.substr(i, len - i);
                    if (wordDict.find(word) != wordDict.end()) {
                        records[len] = true;
                        break;
                    }
                }
            }
        }

        return records[s.size()];
    }
};

int main()
{
    cout << Solution::wordBreak("leetcode", { "leet", "code"}) << endl;
    cout << Solution::wordBreak("leetcode", { "le", "code", "et"}) << endl;
    cout << Solution::wordBreak("leetcode", { "le", "ode", "et"}) << endl;
    cout << Solution::wordBreak("bb", { "a", "b", "bbb"}) << endl;
    cout << Solution::wordBreak("dogs", { "dog", "do", "gs"}) << endl;
    cout << Solution::wordBreak("dcacbcadcad", {"cbd","dca","bcdc","dcac","ad"}) << endl;
    cout << "\n";
    cout << Solution::wordBreak2("leetcode", { "leet", "code"}) << endl;
    cout << Solution::wordBreak2("leetcode", { "le", "code", "et"}) << endl;
    cout << Solution::wordBreak2("leetcode", { "le", "ode", "et"}) << endl;
    cout << Solution::wordBreak2("bb", { "a", "b", "bbb"}) << endl;
    cout << Solution::wordBreak2("dogs", { "dog", "do", "gs"}) << endl;
    cout << Solution::wordBreak2("dcacbcadcad", {"cbd","dca","bcdc","dcac","ad"}) << endl;
}
