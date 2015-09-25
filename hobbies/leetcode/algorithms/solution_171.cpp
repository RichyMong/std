#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int titleToNumber(string s) {
        int num = 0;

        for (auto x : s) {
            num = num * 26 + (x - 'A' + 1) ;
        }

        return num;
    }

};

int main()
{
    cout << Solution::titleToNumber("A") << endl;
    cout << Solution::titleToNumber("Z") << endl;
    cout << Solution::titleToNumber("AA") << endl;
    cout << Solution::titleToNumber("AB") << endl;
}
