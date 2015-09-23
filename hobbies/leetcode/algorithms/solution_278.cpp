#include <iostream>
#include <assert.h>

using namespace std;

int bad = 3;
bool isBadVersion(int version)
{
    cout << __func__ << ": " << version << endl;
    return version >= bad;
}

class Solution {
public:
    static int helper(int start, int end) {
        if (start == end) {
            if (isBadVersion(start) && (start == 0 || !isBadVersion(start - 1))) return start;
        }

        assert(start <= end);

        int mid = (start + end) / 2;
        if (isBadVersion(mid))
            return helper(start, mid);
        else
            return helper(mid + 1, end);
    }

    static int firstBadVersion(int n) {
        return helper(0, n - 1);
    }

    static int firstBadVersion2(int n) {
        int s = 0;
        while (s <= n) {
            if (s == n) {
                if (isBadVersion(s) && !isBadVersion(s - 1)) return s;
            }
            int m = ((long)s + n) / 2; // or int m = s + (n - s) / 2;
            if (isBadVersion(m))
                n = m;
            else
                s = m + 1;
        }
        return 0;
    }

    static int firstBadVersion3(int n) {
        int s = 1;
        while (s < n) {
            int m = ((long)s + n) / 2;
            if (!isBadVersion(m))
                s = m + 1;
            else
                n = m;
        }
        return s;
    }
};

int main()
{
    cout << Solution::firstBadVersion(8) << endl;
    cout << Solution::firstBadVersion2(8) << endl;
    cout << Solution::firstBadVersion3(8) << endl;
    bad = 7;
    cout << Solution::firstBadVersion(8) << endl;
    cout << Solution::firstBadVersion2(8) << endl;
    cout << Solution::firstBadVersion3(8) << endl;
    bad = 2;
    cout << Solution::firstBadVersion(8) << endl;
    cout << Solution::firstBadVersion2(8) << endl;
    cout << Solution::firstBadVersion3(8) << endl;
    bad = 9;
    cout << Solution::firstBadVersion2(8) << endl;
    bad = 1702766719;
    cout << Solution::firstBadVersion2(2126753390) << endl;
    cout << Solution::firstBadVersion3(2126753390) << endl;
}
