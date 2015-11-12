// Count and Say - The count-and-say sequence is the sequence of integers beginning
// as follows:
// 1, 11, 21, 1211, 111221, ...
//
// 1 is read off as "one 1" or 11.
// 11 is read off as "two 1s" or 21.
// 21 is read off as "one 2, then
// one 1" or 1211.
//
// Given an integer n, generate the nth sequence.
//
// // Note: The sequence of integers will be represented as a string.
//
// https://leetcode.com/problems/count-and-say/
#include "solution.h"
#include <sstream>

using namespace std;

class Solution {
public:
    static string countAndSay(int n) {
        if (n <= 1) return string { "1" };

        string s { "1" };

        for (int i = 2; i <= n; ++i) {
            ostringstream oss;

            int count = 1, prev = 0;

            for (int i = 1; i < s.size(); ++i) {
                if (s[i] != s[prev]) {
                    oss << count << s[prev];
                    count = 1;
                    prev = i;
                } else {
                    ++count;
                }
            }

            oss << count << s[prev];
            s = oss.str();
        }

        return s;
    }

};

int main()
{
    cout << Solution::countAndSay(1) << endl;
    cout << Solution::countAndSay(2) << endl;
    cout << Solution::countAndSay(3) << endl;
    cout << Solution::countAndSay(4) << endl;
    cout << Solution::countAndSay(5) << endl;
    cout << Solution::countAndSay(6) << endl;
}
