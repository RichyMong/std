// unique-paths - A robot is located at the top-left corner of a m x n grid
// marked 'Start' in the diagram below).
// The robot can only move either down or right at any point in time. The
// robot is trying to reach the bottom-right corner of the grid (marked
// 'Finish' in the diagram below).
// How many possible unique paths are there?
//
// Above is a 3 x 7 grid. How many possible unique paths are there?
// Note: m and n will be at most 100.
// https://leetcode.com/problems/unique-paths
#include "solution.h"

using namespace std;

class Solution {
public:
    int uniquePaths(int m, int n) {
        vector<vector<int> > uq_at_pos(m, vector<int>(n, 0));
        for (int i = 0; i < m; i++) {
            uq_at_pos[i][n - 1] = 1;
        }

        for (int i = 0; i < n; i++) {
            uq_at_pos[m - 1][i] = 1;
        }

        for (int i = m - 2; i >= 0; i--) {
            for (int j = n - 2; j >= 0; j--) {
                uq_at_pos[i][j] = uq_at_pos[i][j + 1] + uq_at_pos[i + 1][j];
            }
        }

        return uq_at_pos[0][0];
    }
};

int main()
{
    Solution s;
    cout << s.uniquePaths(3, 7) << endl;
    cout << s.uniquePaths(2, 2) << endl;
    cout << s.uniquePaths(2, 3) << endl;
    cout << s.uniquePaths(1, 1) << endl;
}
