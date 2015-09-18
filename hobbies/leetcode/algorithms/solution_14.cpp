#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Solution {
public:
    static string longestCommonPrefix(const vector<string>& strs) {
        if (strs.empty()) return string {};
        auto common = strs[0];
        auto len = common.size();
        for (auto i = 1; i < strs.size(); ++i) {
            auto j = 0;
            while (j < min(len, strs[i].size()) && common[j] == strs[i][j]) {
                j++;
            }

            if (j == 0) return string {};
            if (j < len) len = j;
        }
        return common.substr(0, len);
    }

    static string longestCommonPrefix2(vector<string>& strs) {
        if (strs.empty()) return string {};

        sort(strs.begin(), strs.end());
        auto common = strs[0];
        auto len = common.size();
        for (auto i = 1; i < strs.size(); ++i) {
            int j = 0;
            while (j < min(len, strs[i].size()) && common[j] == strs[i][j]) {
                j++;
            }
            if (j == 0) return string {};
            if (j < len) len = j;
        }

        return common.substr(0, len);
    }

};

int main()
{
    vector<string> s1 { "abc", "ab", "a" };
    vector<string> s2 { "abc", "ab", "a", "def" };
    vector<string> s3 { "abc", "ab", "abdef" };
    cout << Solution::longestCommonPrefix(s1) << endl;
    cout << Solution::longestCommonPrefix(s2) << endl;
    cout << Solution::longestCommonPrefix(s3) << endl;
    cout << Solution::longestCommonPrefix2(s1) << endl;
    cout << Solution::longestCommonPrefix2(s2) << endl;
    cout << Solution::longestCommonPrefix2(s3) << endl;
}
