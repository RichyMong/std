// restore-ip-addresses - Given a string containing only digits, restore it by
// returning all possible valid IP address combinations.
//
// For example:

// / Given "25525511135",
//
// return ["255.255.11.135", "255.255.111.35"].
// (Order does not matter)
// https://leetcode.com/problems/restore-ip-addresses
#include "solution.h"

using namespace std;

class Solution {
    void helper(vector<string>& result, string s, string cur, size_t pos, int fields) {
        if (fields == 4 && pos == s.size()) {
            result.push_back(cur);
            return;
        }

        if (pos >= s.size() || fields >= 4) return;

        if (!cur.empty()) cur += '.';

        if (s[pos] == '0') {
            helper(result, s, cur + s.substr(pos, 1), pos + 1, fields + 1);
            return;
        }

        helper(result, s, cur + s.substr(pos, 1), pos + 1, fields + 1);
        helper(result, s, cur + s.substr(pos, 2), pos + 2, fields + 1);
        auto s3 = s.substr(pos, 3);
        if (stoi(s3) <= 255) {
            helper(result, s, cur + s3, pos + 3, fields + 1);
        }
    }
public:
    vector<string> restoreIpAddresses(string s) {
        vector<string> res;
        helper(res, s, "", 0, 0);
        return res;
    }
};

int main()
{
    Solution sl;
    pretty_print(sl.restoreIpAddresses("25525511135"));
    pretty_print(sl.restoreIpAddresses("25521117"));
    pretty_print(sl.restoreIpAddresses("100211357"));
    pretty_print(sl.restoreIpAddresses("1002113570"));
    pretty_print(sl.restoreIpAddresses("0000"));
}
