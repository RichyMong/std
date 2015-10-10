// Reverse Words in a String
// Given an input string, reverse the string word by word.
//
// For example,
// Given s = "the sky is blue",
// return "blue is sky the".
//
// Update (2015-02-12):
// For C programmers: Try to solve it in-place in O(1) space.
// click to show clarification.
// Clarification:
//
// What constitutes a word?
// A sequence of non-space characters constitutes a word.
// Could the input string contain leading or trailing spaces?
// Yes. However, your reversed string should not contain leading or trailing spaces.
// How about multiple spaces between two words?
// Reduce them to a single space in the reversed string.
//
//
// https://leetcode.com/problems/reverse-words-in-a-string/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static void reverseWords(string &s) {
        int end = -1;
        string result;
        for (int i = (int) s.size() - 1; i >= 0; --i) {
            if (s[i] != ' ' && end < 0) {
                end = i;
            } else if (end >= 0 && s[i] == ' ') {
                if (!result.empty()) result.push_back(' ');
                for (int j = i + 1; j <= end; j++) {
                    result.push_back(s[j]);
                }
                end = -1;
            }
        }

        if (end >= 0) {
            if (!result.empty()) result.push_back(' ');
            for (int j = 0; j <= end; j++) {
                result.push_back(s[j]);
            }
        }
        s = result;
    }
};

int main()
{
    string s = "  abc  def    g   ";
    cout << "before: " << s << endl;
    Solution::reverseWords(s);
    cout << "after: " << s << endl;
    s = "abcdef";
    cout << "before: " << s << endl;
    Solution::reverseWords(s);
    cout << "after: " << s << endl;
    s = "a ";
    cout << "before: " << s << endl;
    Solution::reverseWords(s);
    cout << "after: " << s << ":" << endl;
    s = "   a   b ";
    cout << "before: " << s << endl;
    Solution::reverseWords(s);
    cout << "after: " << s << ":" << endl;
    s = "a";
    cout << "before: " << s << endl;
    Solution::reverseWords(s);
    cout << "after: " << s << ":" << endl;
    s = "the sky is blue";
    cout << "before: " << s << endl;
    Solution::reverseWords(s);
    cout << "after: " << s << ":" << endl;
}
