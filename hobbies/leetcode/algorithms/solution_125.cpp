#include <iostream>
#include <string>
#include <algorithm>
#include <ctype.h>

using namespace std;

class Solution {
public:
    static bool isPalindrome(string s) {
        s.erase(remove_if(s.begin(), s.end(), [](char c) { return !isalnum(c);}), s.end());
        return s.empty() || equal(s.begin(), s.end(), s.rbegin(),
                                 [](char a, char b) { return tolower(a) == tolower(b); });
    }

    static bool isPalindrome2(string s) {
        for (int i = 0, j = s.size() - 1; i < j; i++, j--) {
            while (!isalnum(s[i]) && i < j) i++;
            while (!isalnum(s[j]) && j > i) j--;
            if (tolower(s[i]) != tolower(s[j])) return false;
        }
        return true;
    }

    static bool isPalindrome3(string s) {
        auto e = remove_if(s.begin(), s.end(), [](char c) { return !isalnum(c);});

        auto new_rend = string::reverse_iterator(e);

        return s.empty() || equal(s.begin(), new_rend.base(), new_rend,
                                 [](char a, char b) { return tolower(a) == tolower(b); });
    }
};

int main()
{
    string s1 = "A man, a plan, a canal: Panama";
    cout << Solution::isPalindrome(s1) << endl;
    cout << Solution::isPalindrome2(s1) << endl;
    cout << Solution::isPalindrome3(s1) << endl;
    string s2 = "race a car";
    cout << Solution::isPalindrome(s2) << endl;
    cout << Solution::isPalindrome2(s2) << endl;
    cout << Solution::isPalindrome3(s2) << endl;
    string s3 = "";
    cout << Solution::isPalindrome(s3) << endl;
    cout << Solution::isPalindrome2(s3) << endl;
    cout << Solution::isPalindrome3(s3) << endl;
    string s4 = "abcde,dc:ba";
    cout << Solution::isPalindrome(s4) << endl;
    cout << Solution::isPalindrome2(s4) << endl;
    cout << Solution::isPalindrome3(s4) << endl;
}
