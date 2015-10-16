// Integer to Roman - Given an integer, convert it to a roman numeral. Input
// is guaranteed to be within the range from 1 to 3999.

// https://leetcode.com/problems/integer-to-roman/
#include "solution.h"

using namespace std;

class Solution {
public:
    static string intToRoman(int num) {
        map<int, string> table {
            { 1000, "M" }, { 900, "CM" }, { 500, "D" }, { 400, "CD" },
            { 100, "C" }, { 90, "XC" }, { 50, "L" }, { 40, "XL" },
            { 10, "X" }, { 9, "IX" }, { 5, "V" }, { 4, "IV" }, { 1, "I" },
        };

        string result;

        auto it = table.rbegin();
        while (num > 0) {
            while (num < it->first)
                ++it;
            num -= it->first;
            result += it->second;
        }

        return result;
    }

};

int main()
{
    cout << Solution::intToRoman(1) << endl;
    cout << Solution::intToRoman(4) << endl;
    cout << Solution::intToRoman(5) << endl;
    cout << Solution::intToRoman(9) << endl;
    cout << Solution::intToRoman(10) << endl;
    cout << Solution::intToRoman(32) << endl;
    cout << Solution::intToRoman(40) << endl;
    cout << Solution::intToRoman(50) << endl;
    cout << Solution::intToRoman(90) << endl;
    cout << Solution::intToRoman(1990) << endl;
    cout << Solution::intToRoman(2014) << endl;
}
