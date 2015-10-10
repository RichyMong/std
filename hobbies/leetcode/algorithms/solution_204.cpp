// Count Primes - Description:
// Count the number of prime numbers less than a non-negative number, n.


// https://leetcode.com/problems/count-primes/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int countPrimes(int n) {
        vector<bool> primes(n, true);

        for (int i = 2; i * i < n; ++i) {
            if (!primes[i]) continue;

            for (int j = i * i; j < n; j += i) {
                primes[j] = false;
            }
        }

        int count = 0;
        for (int i = 2; i < n; ++i) {
            if (primes[i]) ++count;
        }

        return count;
    }

    static int countPrimes2(int n) {
        vector<bool> primes(n, true);

        int count = 0;

        for (int i = 2; i < n; ++i) {
            if (!primes[i]) continue;

            ++count;

            for (int j = i * i; j < n; j += i) {
                primes[j] = false;
            }
        }

        return count;
    }
};

int main()
{
    cout << Solution::countPrimes(213) << endl;
    cout << Solution::countPrimes2(213) << endl;
}
