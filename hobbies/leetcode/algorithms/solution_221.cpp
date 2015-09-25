#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int maximalSquare(const vector<vector<char>>& matrix) {
        const auto rows = matrix.size();

        if (rows == 0) return 0;

        const auto cols = matrix[0].size();

        vector<vector<char>> result(rows, vector<char>(cols));

        for (int i = 0; i < cols; ++i) {
            result[0][i] = matrix[0][i] - '0';
        }

        for (int i = 0; i < rows; ++i) {
            result[i][0] = matrix[i][0] - '0';
        }

        for (int i = 1; i < rows; ++i) {
            for (int j = 1; j < cols; ++j) {
                if (matrix[i][j] == '0')
                    result[i][j] = 0;
                else
                    result[i][j] = min(min(result[i - 1][j], result[i][j - 1]), result[i - 1][j - 1]) + 1;
            }
        }

        int max_i = 0, max_j = 0;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (result[i][j] >= result[max_i][max_j]) {
                    max_i = i;
                    max_j = j;
                }
            }
        }

        return result[max_i][max_j] * result[max_i][max_j];
    }

};

int main()
{
    vector<vector<char>> v = {
        {'1','0','1','0','0'},
        {'1','0','1','1','1'},
        {'1','1','1','1','1'},
        {'1','0','0','1','0'},
    };

    cout << Solution::maximalSquare(v) << endl;
    cout << Solution::maximalSquare({{'0'}}) << endl;
    cout << Solution::maximalSquare({{'1'}}) << endl;
}
