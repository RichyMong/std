#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Solution {
public:
    static string longestCommonPrefix(const vector<string>& strs) {
        if (strs.empty()) return "";

        auto& common = strs[0];
        auto len = common.size();
        for (auto i = 1; i < strs.size(); ++i) {
            auto j = 0;
            while (j < len && j < strs[i].size() && common[j] == strs[i][j]) {
                j++;
            }

            if (j == 0) return "";
            if (j < len) len = j;
        }
        return common.substr(0, len);
    }

    static string longestCommonPrefix2(vector<string>& strs) {
        if (strs.empty()) return string {};

        sort(strs.begin(), strs.end());

        auto& common = strs[0];
        int len = strs[0].size();

        for (int i = 1; i < strs.size(); ++i) {
            int j = 0;
            while (j < min(len, (int)strs[i].size())) {
                if (strs[i][j] != common[j])
                    break;
                ++j;
            }
            if (j == 0) return string {};
            len = j;
        }

        return common.substr(0, len);
    }

    static string longestCommonPrefix3(vector<string>& strs) {
        if (strs.empty()) return "";

        string prefix = "";
        for (int i = 0; i < strs[0].size(); prefix += strs[0][i++]) {
            for (int j = 1; j < strs.size(); ++j) {
                if (strs[j][i] != strs[0][i]) return prefix;
            }
        }

        return prefix;
    }

};

int main()
{
    vector<string> s1 { "abc", "ab", "a" };
    vector<string> s2 { "abc", "ab", "a", "def" };
    vector<string> s3 { "abc", "ab", "abdef" };
    vector<string> s4 { "abc", "", "ab", "abdef" };
    cout << Solution::longestCommonPrefix(s1) << endl;
    cout << Solution::longestCommonPrefix(s2) << endl;
    cout << Solution::longestCommonPrefix(s3) << endl;
    cout << Solution::longestCommonPrefix3(s1) << endl;
    cout << Solution::longestCommonPrefix3(s2) << endl;
    cout << Solution::longestCommonPrefix3(s3) << endl;
    cout << Solution::longestCommonPrefix3(s4) << endl;
}
