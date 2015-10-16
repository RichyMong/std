// Roman to Integer - Given a roman numeral, convert it to an integer.
// Input
// is guaranteed to be within the range from 1 to 3999.
// https://leetcode.com/problems/roman-to-integer/
#include "solution.h"

using namespace std;

class Solution {
public:
    static int romanToInt(string s) {
        unordered_map<char, int> table {
            { 'I', 1 }, { 'V', 5 }, { 'X', 10 },
            { 'L', 50 }, { 'C', 100 }, { 'D', 500 }, { 'M', 1000 }
        };

        auto result = 0, sign = 1;

        for (int i = 0; i < s.size(); ++i) {
            if (i == 0 && s[i] == '-') {
                sign = -1;
                continue;
            }

            if (i > 0 && table[s[i]] > table[s[i - 1]]) {
                result += table[s[i]] - 2 * table[s[i - 1]];
            } else {
                result += table[s[i]];
            }
        }

        return result * sign;
    }
};

int main()
{
    cout << Solution::romanToInt("IV") << endl;
    cout << Solution::romanToInt("IX") << endl;
    cout << Solution::romanToInt("CCVII") << endl;
    cout << Solution::romanToInt("MLXVI") << endl;
    cout << Solution::romanToInt("MCMLIV") << endl;
    cout << Solution::romanToInt("MCMXC") << endl;
    cout << Solution::romanToInt("MMXIV") << endl;
}
