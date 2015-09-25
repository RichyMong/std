#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int lengthOfLastWord(string s) {
        int cend = -1;

        for (int i = (int)s.size() - 1; i >= 0; --i) {
            if (s[i] != ' ' && cend < 0)
                cend = i;

            if (s[i] == ' ' && cend >= 0) {
                return cend - i;
            }
        }

        return cend < 0 ? 0 : cend + 1;
    }

    static int lengthOfLastWord2(string s) {
        auto pos = s.find_last_not_of(" ");
        auto end = s.find_last_of(" ", pos);

        return pos == string::npos ? 0 : (end == string::npos ? pos + 1 : pos - end) ;
    }
};

int main()
{
    cout << Solution::lengthOfLastWord2("hello world") << endl;
    cout << Solution::lengthOfLastWord2("   ") << endl;
    cout << Solution::lengthOfLastWord2("") << endl;
    cout << Solution::lengthOfLastWord2("abc") << endl;
    cout << Solution::lengthOfLastWord2("a ") << endl;
}
