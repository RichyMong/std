#include <iostream>
#include <string>

using namespace std;

class Solution {
public:
    static int compareVersion(string version1, string version2) {
        string::size_type op1 = 0;
        string::size_type op2 = 0;

        while (op1 < version1.size() || op2 < version2.size()) {
            auto v1 = 0;
            auto v2 = 0;
            if (op1 != string::npos) {
                auto p1 = version1.find('.', op1 + 1);
                if (p1 != string::npos) {
                    v1 = stoi(string(version1, op1, p1 - op1 + 1));
                    op1 = p1 + 1;
                } else {
                    v1 = stoi(string(version1, op1, string::npos));
                    op1 = string::npos;
                }

            }
            if (op2 != string::npos) {
                auto p2 = version2.find('.', op2 + 1);
                if (p2 != string::npos) {
                    v2 = stoi(string(version2, op2, p2 - op2 + 1));
                    op2 = p2 + 1;
                } else {
                    v2 = stoi(string(version2, op2, string::npos));
                    op2 = string::npos;
                }
            }
            if (v1 > v2) return 1;
            else if (v1 < v2) return -1;
        }

        return 0;
    }

    static int compareVersion2(string version1, string version2) {
        auto i = 0, j = 0;
        while (i < version1.size() || j < version2.size()) {
            auto v1 = 0, v2 = 0;
            while (i < version1.size() && isdigit(version1[i])) {
                v1 = v1 * 10 + (version1[i] - '0');
                ++i;
            }

            while (j < version2.size() && isdigit(version2[j])) {
                v2 = v2 * 10 + (version2[j] - '0');
                ++j;
            }

            if (v1 < v2) return -1;
            else if (v1 > v2) return 1;
            ++i;
            ++j;
        }

        return 0;
    }
};

int main()
{
    cout << Solution::compareVersion("0.1", "1.2") << endl;
    cout << Solution::compareVersion("1.1", "1.0") << endl;
    cout << Solution::compareVersion("1", "1.0") << endl;
    cout << Solution::compareVersion2("1.1", "1.01") << endl;
    cout << Solution::compareVersion2("1.0.9", "1.00.9") << endl;
    cout << Solution::compareVersion2("0.1", "1.2") << endl;
    cout << Solution::compareVersion2("1.1", "1.0") << endl;
    cout << Solution::compareVersion2("1", "1.0") << endl;
    cout << Solution::compareVersion2("1.1", "1.01") << endl;
    cout << Solution::compareVersion2("1.0.9", "1.00.9") << endl;
}
