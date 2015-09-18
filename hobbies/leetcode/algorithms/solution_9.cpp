#include <iostream>
#include <math.h>

using namespace std;

class Solution {
public:
    static bool isPalindrome(int x) {
        if (x < 0 || (x && (x % 10) == 0)) return false;

        int half = 0;

        while (x > half){
            half = half * 10 + (x % 10);
            x /= 10;
        }

        return x == half || x == half / 10;
    }

};

int main()
{
    cout << Solution::isPalindrome(0) << endl;
    cout << Solution::isPalindrome(10) << endl;
    cout << Solution::isPalindrome(101) << endl;
    cout << Solution::isPalindrome(1101) << endl;
    cout << Solution::isPalindrome(11011) << endl;
    cout << Solution::isPalindrome(11) << endl;
    cout << Solution::isPalindrome(1001) << endl;
}
