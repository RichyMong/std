// powx-n - Implement pow(x, n).
// https://leetcode.com/problems/powx-n
#include "solution.h"
#include <limits>

using namespace std;

class Solution {
public:
    double myPow(double x, int n) {
        if (n == numeric_limits<int>::min()) {
            if (x == 1) return x;
            else if (x == -1) return -x;
            else return 0;
        } else if (n == numeric_limits<int>::max()) {
            if (x == 1 || x == -1) return x;
            else return 0;
        }

        if (n == 0) return 1.0;
        else if (n < 0) return 1.0 / myPow(x, -n);

        auto half = myPow(x, n / 2);
        if (n % 2 == 0) {
            return half * half;
        } else {
            return half * half * x;
        }
    }
};

int main()
{
    cout << Solution().myPow(10, 3) << endl;
    cout << Solution().myPow(1, numeric_limits<int>::min()) << endl;
}
