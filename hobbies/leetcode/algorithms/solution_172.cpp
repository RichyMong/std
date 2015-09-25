#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int trailingZeroes(int n) {
        int count = 0;
        while (n >= 5) {
            n /= 5;
            count += n;
        }

        return count;
    }
};

int main()
{
    cout << Solution::trailingZeroes(10) << endl;
    cout << Solution::trailingZeroes(20) << endl;
    cout << Solution::trailingZeroes(90) << endl;
    cout << Solution::trailingZeroes(100) << endl;
}
