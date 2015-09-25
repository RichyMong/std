#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static string convert(string s, int numRows) {
        if (numRows < 2) return s;

        string result;

        for (int i = 1; i <= numRows; ++i) {
            bool flag = true;
            for (int j = i - 1; j < s.size(); ) {
                result.push_back(s[j]);
                if (i == 1 || i == numRows) {
                    j += 2 * (numRows - 1);
                } else {
                    j += 2 * (flag ? (numRows - i) : i - 1);
                    flag = !flag;
                }
            }
        }

        return result;
    }
};

int main()
{
    cout << Solution::convert("PAYPALISHIRING", 3) << endl;
    cout << Solution::convert("PAYPALISHIRING", 4) << endl;
    cout << Solution::convert("ABCDE", 4) << endl;
}
