#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int strStr(string haystack, string needle) {
        const int m = needle.size();

        vector<int> table(m, -1);
        int i = 0, j = -1;
        while (i < m - 1) {
            while (j >= 0 && needle[i] != needle[j])
                j = table[j];
            ++i, ++j;
            table[i] = j;
        }

        i = j = 0;
        while (i < haystack.size()) {
            while (j >= 0 && haystack[i] != needle[j])
                j = table[j];
            ++i, ++j;
            if (j == m) {
                return i - j;
            }
        }

        return -1;
    }
};

int main()
{
    cout << Solution::strStr("abc abcdeabcdf", "abcdf") << endl;
    cout << Solution::strStr("abcabcabd", "abcabd") << endl;
}
