// longest-valid-parentheses - Given a string containing just the characters '(
//  and ')', find the length of the longest valid (well-formed) parentheses sub-
// string.
//
// For "(()", the longest valid parentheses substring is "()",
// which has length = 2.
//
// Another example is ")()())", where the longest
// valid parentheses substring is "()()", which has length = 4.
// https://leetcode.com/problems/longest-valid-parentheses
#include "solution.h"
#include <stack>

using namespace std;

class Solution {
public:
    int longestValidParentheses(const string& s) {
        if (s.empty()) {
            return 0;
        }

        vector<int> vec(s.size());
        stack<char> cstack;
        cstack.push(s[0]);

        int last_close = -1;
        int max_length = 0;
        for (size_t i = 1; i < s.size(); i++) {
            if (s[i] == ')') {
                if (!cstack.empty() && cstack.top() == '(') {
                    if (last_close > 0) {
                        vec[i] = vec[last_close] + 2;
                    } else {
                        vec[i] = 2;
                        last_close = i;
                    }
                    if (vec[i] > max_length) {
                        max_length = vec[i];
                    }
                    cstack.pop();
                } else {
                    vec[i] = 0;
                    cstack = stack<char>{};
                }
            } else if (s[i] == '(') {
                cstack.push(s[i]);
            }
        }

        return max_length;;
    }
};

int main()
{
    cout << Solution().longestValidParentheses("()()") << endl;
    cout << Solution().longestValidParentheses(")()") << endl;
    cout << Solution().longestValidParentheses("((()))") << endl;
    cout << Solution().longestValidParentheses("((())(") << endl;
    cout << Solution().longestValidParentheses("(((())))") << endl;
    cout << Solution().longestValidParentheses("(())()(())") << endl;
}
