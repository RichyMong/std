#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>

using namespace std;

class Solution {
public:
    static bool isValid(const string& s) {
        if (s.size() % 2 != 0) return false;

        stack<char> sc;
        for (auto c : s) {
            switch (c) {
            case '(': case '[': case '{':
                sc.push(c);
                break;
            case ')': case ']': case '}':
                if (sc.empty()) return false;
                if ((c == ')' && sc.top() != '(') ||
                    (c == ']' && sc.top() != '[') ||
                    (c == '}' && sc.top() != '{'))
                    return false;
                sc.pop();
                break;
            default:
                return false;
            }
        }
        return sc.empty();
    }

    static bool isValid2(const string& s) {
        if (s.size() % 2 != 0) return false;

        stack<char> left;
        unordered_map<char, char> pm { {')', '('}, {']', '['}, {'}', '{'}};
        for (int i = 0; i < s.size(); ++i) {
            auto f = pm.find(s[i]);
            if (f != pm.end()) {
                if (left.empty() || left.top() != f->second)
                    return false;
                left.pop();
            } else {
                left.push(s[i]);
            }
        }

        return left.empty();
    }

    static bool isValid3(const string& t) {
        string s = t;
        if (s.size() % 2 != 0) return false;

        while (!s.empty()) {
            auto l = s.find("()");
            if (l != string::npos)
                s.erase(l, 2);

            auto m = s.find("[]");
            if (m != string::npos)
                s.erase(m, 2);

            auto b = s.find("{}");
            if (b != string::npos)
                s.erase(b, 2);

            if (l == string::npos && m == string::npos && b == string::npos)
                return false;
        }

        return true;
    }
};

int main()
{
    cout << Solution::isValid("(){}") << endl;
    cout << Solution::isValid("({)}") << endl;
    cout << Solution::isValid("({})") << endl;
    cout << Solution::isValid("({)") << endl;
    cout << Solution::isValid("(((()())))") << endl;
    cout << Solution::isValid2("(){}") << endl;
    cout << Solution::isValid2("({)}") << endl;
    cout << Solution::isValid2("({})") << endl;
    cout << Solution::isValid2("({)") << endl;
    cout << Solution::isValid2("(((()())))") << endl;
    cout << Solution::isValid3("(){}") << endl;
    cout << Solution::isValid3("({)}") << endl;
    cout << Solution::isValid3("({})") << endl;
    cout << Solution::isValid3("({)") << endl;
    cout << Solution::isValid3("(((()())))") << endl;
}
