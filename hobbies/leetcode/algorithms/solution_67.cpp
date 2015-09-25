#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static string addBinary(string a, string b) {
        string result = "";

        int i = a.size() - 1, j = b.size() - 1, flag = 0;
        while (i >= 0 || j >= 0 || flag) {
            flag += (i >= 0 ? a[i--] - '0' : 0);
            flag += (j >= 0 ? b[j--] - '0' : 0);
            result = static_cast<char>(flag % 2 + '0') + result;
            flag /= 2;
        }
        return result;
    }

    static string helper(const string& a, const string& b, int flag) {
        if (a.empty() && b.empty()) return flag ? to_string(flag) : "";

        if (a.empty()) return helper(string(b.size(), '0'), b, flag);
        if (b.empty()) return helper(a, string(a.size(), '0'), flag);

        auto sum = a.back() - '0' + b.back() - '0' + flag;

        return helper(a.substr(0, a.size() - 1), b.substr(0, b.size() - 1), sum / 2) + static_cast<char>(sum % 2 + '0');
    }

    static string addBinary2(string a, string b) {
        return helper(a, b, 0);
    }
};

int main()
{
    cout << Solution::addBinary("100", "111") << endl;
    cout << Solution::addBinary("10100", "111") << endl;
    cout << Solution::addBinary2("100", "111") << endl;
    cout << Solution::addBinary2("10100", "111") << endl;
}
