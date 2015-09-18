#include <iostream>
#include <string>
#include <set>
#include <algorithm>

using namespace std;

class Solution {
public:
    static bool isAnagram(const string& s, const string& t) {
        return multiset<char>(s.begin(), s.end()) == multiset<char>(t.begin(), t.end());
    }
    static bool isAnagram2(string s, string t) {
        sort(s.begin(), s.end());
        sort(t.begin(), t.end());
        return s == t;
    }
};

int main()
{
    cout << Solution::isAnagram("abdc", "badc") << endl;
    cout << Solution::isAnagram("car", "rat") << endl;
    cout << Solution::isAnagram("aa", "a") << endl;

    string s1 = "abdc", s2 = "badc";
    cout << Solution::isAnagram2(s1, s2) << endl;
    s1 = "car"; s2 = "rat";
    cout << Solution::isAnagram2(s1, s2) << endl;
    s1 = "aa"; s2 = "a";
    cout << Solution::isAnagram2(s1, s2) << endl;
}
