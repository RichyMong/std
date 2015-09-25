#include "solution.h"
#include <algorithm>

using namespace std;

class Solution {
public:
    static string convertToTitle(int n) {
        string s;

        while (n > 0) {
            s += (n - 1) % 26 + 'A';
            n = (n - 1) / 26;
        }

        reverse(s.begin(), s.end());
        return s;
    }
};

int main()
{
    cout << Solution::convertToTitle(1) << endl;
    cout << Solution::convertToTitle(26) << endl;
    cout << Solution::convertToTitle(27) << endl;
    cout << Solution::convertToTitle(28) << endl;
    cout << Solution::convertToTitle(54) << endl;
}
