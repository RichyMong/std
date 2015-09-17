#include <iostream>
#include <string>
#include <stack>
#include <ctype.h>
#include <assert.h>

using namespace std;

class Solution {
public:
    static int prim(const string& s, size_t& pos) {
        assert (pos < s.size());

        while (pos < s.size()) {
            switch (s[pos]) {
                case '0': case '1' : case '2': case '3':
                case '4': case '5' : case '6': case '7':
                case '8' : case '9': {
                    size_t idx = 0;
                    auto ret = stol(s.substr(pos), &idx);
                    pos += idx;
                    return ret;
                }

                case '-':
                    return -prim(s, ++pos);

                case '(':
                    {
                        ++pos;
                        auto ret = expr(s, pos);
                        assert(s[pos] == ')');
                        ++pos;
                        return ret;
                    }

                default:
                    break;
            }
            ++pos;
        }

        return 0;
    }

    static int expr(const string& s, size_t& pos) {
        if (pos >= s.size()) {
            return 0;
        }

        auto result = prim(s, pos);
        for ( ; ; ) {
            auto c = s[pos];
            switch (c) {
            case '+':
                result += prim(s, ++pos);
                break;

            case '-':
                result -= prim(s, ++pos);
                break;

            case ' ':
                pos += 1;
                break;

            default:
                return result;
            }
        }
    }

    static int calculate(const string& s) {
        size_t pos = 0;
        return expr(s, pos);
    }

    static int calculate2(string s) {
        stack<int> prims, ops;

        auto result = 0;
        auto sign = 1;
        auto num = 0;

        for (auto c : s) {
            if (isdigit(c)) {
                num = num * 10 + (c - '0');
            } else {
                result += sign * num;
                num = 0;
                if (c == '+') sign = 1;
                if (c == '-') sign = -1;
                if (c == '(') {
                    prims.push(result);
                    ops.push(sign);
                    result = 0;
                    sign = 1;
                }
                if (c == ')') {
                    result = prims.top() + ops.top() * result;
                    prims.pop();
                    ops.pop();
                }
            }
        }

        result += sign * num;
        return result;
    }
};

int main()
{
    string abc = "123 + 456";
    cout << Solution::calculate(abc) << endl;
    cout << Solution::calculate("(1 + 2) + 3 - (2 + 4)") << endl;
    cout << Solution::calculate("(1 + 2) + 3 - ((2 + 4) - 8)") << endl;
    cout << Solution::calculate("-1 + 3") << endl;
    cout << Solution::calculate("+1 + 3") << endl;
    cout << Solution::calculate2(abc) << endl;
    cout << Solution::calculate2("(1 + 2) + 3 - (2 + 4)") << endl;
    cout << Solution::calculate2("(1 + 2) + 3 - ((2 + 4) - 8)") << endl;
    cout << Solution::calculate2("-1 + 3") << endl;
    cout << Solution::calculate2("+1 + 3") << endl;
}
