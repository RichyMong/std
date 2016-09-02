// Generate Parentheses - Given n pairs of parentheses, write a function to
// generate all combinations of well-formed parentheses.
//
// For example, given n = 3, a solution set is:
//
// [
//   "((()))",
//   "(()())",
//   "(())()",
//   "()(())",
//   "()()()"
// ]
// https://leetcode.com/problems/generate-parentheses/
#include "solution.h"

using namespace std;

class Solution {
public:
    void helper(int left, int right, const string& current, vector<string>& res) {
        if (!left && !right) {
            res.push_back(current);
            return;
        }

        if (left > 0) {
            helper(left - 1, right, current + '(', res);
        }

        if (left < right) {
            helper(left, right - 1, current + ')', res);
        }
    }

    vector<string> generateParentheses(int n) {
        vector<string> res;
        helper(n, n, "", res);

        return res;
    }
};

int main(int argc, char* argv[])
{
    pretty_print(Solution().generateParentheses(argc > 1 ? stoi(argv[1]) : 3));
    return 0;
}
