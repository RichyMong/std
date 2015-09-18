#include <iostream>
#include <string>

using namespace std;

class Solution {
public:
    static int myAtoi(const string& s) {
        auto i = s.find_first_not_of("\r\n\t ");
        if (i == string::npos) {
            return 0;
        }

        int sign = s[i] == '-' ? -1 : 1;
        if (s[i] == '+' || s[i] == '-')
            i++;

        long v = 0;

        for (auto j = i; j <= i + 10 && j < s.size(); ++j) {
            if (!isdigit(s[j])) break;
            v = v * 10 + (s[j] - '0');
        }

        v *= sign;

        if (v > 2147483647) return 2147483647;
        else if (v < -2147483648) return -2147483648;
        return v;
    }

};

int main()
{
    cout << Solution::myAtoi("   -100") << endl;
    cout << Solution::myAtoi("   +23456") << endl;
    cout << Solution::myAtoi("   +1234567890") << endl;
    cout << Solution::myAtoi("   21234567890") << endl;
    cout << Solution::myAtoi("   -2147483648") << endl;
    cout << Solution::myAtoi("2147483647") << endl;
}
