// sqrtx -
// https://leetcode.com/problems/sqrtx
#include "solution.h"
#include <limits>

using namespace std;

class Solution {
public:
    int mySqrt(int x) {
        if (x < 0) return numeric_limits<int>::min();

        long root = x;

        while (root * root > x) {
            root = (root + x / root) / 2;
        }


        return root;
    }
};

int main(int argc, char** argv)
{
    cout << Solution().mySqrt(argc > 1 ? stoi(argv[1]) : 28) << endl;
    return 0;
}
