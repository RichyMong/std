// Longest Palindromic Substring - Given a string S, find the longest palindromic
// substring in S. You may assume that the maximum length of S is 1000, and there
// exists one unique longest palindromic substring.
// https://leetcode.com/problems/longest-palindromic-substring/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static string longestPalindrome(const string& s) {
        if (s.empty()) return string {};

        int max_len = 1;
        int start = 0;

        for (auto i = 1; i < s.size(); ++i) {
            auto low = i - 1;
            auto high = i;

            while (low >= 0 && high < s.size() && s[low] == s[high]) {
                if (high - low + 1 > max_len) {
                    max_len = high - low + 1;
                    start = low;
                }
                --low;
                ++high;
            }

            low = i - 1;
            high = i + 1;
            while (low >= 0 && high < s.size() && s[low] == s[high]) {
                if (high - low + 1 > max_len) {
                    max_len = high - low + 1;
                    start = low;
                }
                --low;
                ++high;
            }
        }

        return s.substr(start, max_len);
    }

    static string longestPalindrome2(const string& s) {
        if (s.empty()) return string {};

        string r(s.rbegin(), s.rend());

        vector<vector<int>> records(s.size(), vector<int>(r.size()));

        int max_i = 0, max_j = 0;

        for (auto i = 0; i < s.size(); ++i) {
            for (auto j = 0; j < r.size(); ++j) {
                if (i == 0 || j == 0) {
                    records[i][j] = (s[i] == r[j]) ? 1 : 0;
                } else if (s[i] == r[j]) {
                        records[i][j] = records[i - 1][j - 1] + 1;
                } else {
                    records[i][j] = 0;
                }

                if (records[i][j] > records[max_i][max_j]) {
                    max_i = i;
                    max_j = j;
                }
            }
        }

        return s.substr(max_i + 1 - records[max_i][max_j], records[max_i][max_j]);
    }

    static string longestPalindrome3(const string& s) {
        string extended(2 * s.size() + 1, '#');

        for (int i = 0; i < s.size(); ++i) {
            extended[2 * i + 1] = s[i];
        }

        vector<int> records(extended.size());

        auto c = 0, r = 0, max_pos = 0;

        for (auto i = 1; i < extended.size(); ++i) {
            if (r > i) {
                records[i] = min(records[2 * c - i], r - i);
            } else {
                records[i] = 0;
            }

            for (auto j = records[i] + 1;
                     i - j >=0 &&
                     i + j < extended.size() &&
                     extended[i - j] == extended[i + j];
                    ++j)
                ++records[i];

            if (records[i] + i > r) {
                r = records[i] + i;
                c = i;
            }

            if (records[i] > records[max_pos]) {
                max_pos = i;
            }
        }

        string result;

        copy_if(extended.begin() + max_pos - records[max_pos],
                extended.begin() + max_pos + records[max_pos],
                back_inserter(result),
                [](char c) { return c != '#'; });

        return result;
    }
};

int main()
{
    cout << Solution::longestPalindrome3("abcba") << endl;
    cout << Solution::longestPalindrome3("abbda") << endl;
    cout << Solution::longestPalindrome3("abbada") << endl;
    cout << Solution::longestPalindrome3("abaa") << endl;
    cout << Solution::longestPalindrome3("aaaaaaaaa") << endl;
    cout << Solution::longestPalindrome3("a") << endl;
    cout << Solution::longestPalindrome3("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz") << endl;
}
