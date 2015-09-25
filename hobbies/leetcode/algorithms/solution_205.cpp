#include <iostream>
#include <unordered_map>
#include "solution.h"

using namespace std;

class Solution {
public:
    static bool isIsomorphic(string s, string t) {
        unordered_map<char, char> s2t;
        unordered_map<char, char> t2s;

        for (int i = 0; i < s.size(); ++i) {
            auto it1 = s2t.find(s[i]);
            if (it1 == s2t.end())
                s2t[s[i]] = t[i];
            else if (it1->second != t[i])
                return false;

            auto it2 = t2s.find(t[i]);
            if (it2 == t2s.end())
                t2s[t[i]] = s[i];
            else if (it2->second != s[i])
                return false;
        }

        return true;
    }
};

int main()
{
    cout << Solution::isIsomorphic("egg", "add") << endl;
    cout << Solution::isIsomorphic("foo", "bar") << endl;
    cout << Solution::isIsomorphic("paper", "title") << endl;
    cout << Solution::isIsomorphic("a a ", "cbcb") << endl;
}
