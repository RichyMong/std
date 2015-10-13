// Regular Expression Matching
// '.' Matches any single character.
// '*' Matches zero or more of the preceding element.
//
// The matching should cover the entire input string (not partial).
//
// The function prototype should be:
// bool isMatch(const char *s, const char *p)
//
// Some examples:
// isMatch("aa","a") → false
// isMatch("aa","aa") → true
// isMatch("aaa","aa") → false
// isMatch("aa", "a*") → true
// isMatch("aa", ".*") → true
// isMatch("ab", ".*") → true
// isMatch("aab", "c*a*b") → true
// https://leetcode.com/problems/regular-expression-matching/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static bool match(const string& s, int spos, const string& p, int ppos) {
        int ssize = s.size(), psize = p.size();

        if (ppos == psize)
            return spos == ssize;

        if (ppos < psize - 1 && p[ppos + 1] == '*')
            return matchStar(s, spos, p, ppos + 2);

        if  (spos < s.size() && (p[ppos] == '.' || p[ppos] == s[spos]))
            return match(s, spos + 1, p, ppos + 1);

        return false;
    }

    static bool matchStar(const string& s, int spos, const string& p, int ppos) {
        char c = p[ppos - 2];

        do {
            if (match(s, spos, p, ppos))
                return true;
        }  while (spos < s.size() && (s[spos++] == c || c == '.'));

        return false;
    }

    static bool isMatch(string s, string p)  {
        return match(s, 0, p, 0);
    }

    static bool isMatch2(string s, string p) {
        auto i = 0, j = 0;

        while (i < p.size()) {
            if (i < p.size() - 1 && p[i + 1] == '*'){
                do {
                    if (isMatch(s.substr(j), p.substr(i + 2))) {
                        return true;
                    }
                } while (j < s.size() && (s[j++] == p[i] || p[i] == '.'));
            }

            if (j >= s.size() || (s[j] != p[i] && p[i] != '.'))
                break;

            ++i, ++j;
        }

        return i == p.size() && j == s.size();
    }
};

int main()
{
    cout << Solution::isMatch2("aa", "a")  << endl;
    cout << Solution::isMatch2("aa", "aa")  << endl;
    cout << Solution::isMatch2("aa", "aaa")  << endl;
    cout << Solution::isMatch2("aa", "a*")  << endl;
    cout << Solution::isMatch2("ab", ".*")  << endl;
    cout << Solution::isMatch2("aa", "b*a*")  << endl;
    cout << Solution::isMatch2("aa", "b*a*c")  << endl;
    cout << Solution::isMatch2("aa", "")  << endl;
}
