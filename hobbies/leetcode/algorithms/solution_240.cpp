#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static bool searchHelper2(const vector<int>& nums, int target, int start, int end) {
        while (start < end) {
            int mid = start + (end - start) / 2;
            if (nums[mid] == target) return true;
            else if (nums[mid] > target) end = mid - 1;
            else start = mid + 1;
        }

        return nums[start] == target;
    }

    static bool helper(const vector<vector<int>>& matrix, int target, int top, int left, int bottom, int right) {
        if (top > bottom || left > right) return false;
        if (top == bottom && left == right) return matrix[top][left] == target;

        int rmid = top + (bottom - top) / 2;
        int cmid = left + (right - left) / 2;

        if (matrix[rmid][cmid] == target) {
            return true;
        } else if (matrix[rmid][cmid] < target) {
            return helper(matrix, target, top, cmid + 1, rmid, right) ||
                   helper(matrix, target, rmid + 1, left, bottom, right);
        } else {
            return helper(matrix, target, top, left, rmid - 1, right) ||
                   helper(matrix, target, rmid, left, bottom, cmid - 1);
        }
    }

    static bool searchMatrix(const vector<vector<int>>& matrix, int target) {
        if (matrix.empty() || matrix[0].empty()) return false;

        return helper(matrix, target, 0, 0, (int)matrix.size() - 1, (int) matrix[0].size() - 1);
    }

    static bool searchMatrix2(const vector<vector<int>>& matrix, int target) {
        if (matrix.empty() || matrix[0].empty()) return false;

        int i = 0, j = matrix[0].size() - 1;

        while (i < matrix.size() && j >= 0) {
            if (matrix[i][j] == target) {
                return true;
            } else if (matrix[i][j] < target) {
                ++i;
            } else {
                --j;
            }
        }

        return false;
    }
};

int main()
{
    vector<vector<int>> v = {
        {1,   4,  7, 11, 15},
        {2,   5,  8, 12, 19},
        {3,   6,  9, 16, 22},
        {10, 13, 14, 17, 24},
        {18, 21, 23, 26, 30}
    };

    vector<vector<int>> v2 = {
        {1,3,5,7,9},
        {2,4,6,8,10},
        {11,13,15,17,19},
        {21,22,23,24,25}
    };

    cout << Solution::searchMatrix(v2, 13) << " " << Solution::searchMatrix2(v2, 13) << endl;
    cout << Solution::searchMatrix(v, 1) << " " << Solution::searchMatrix2(v, 1) << endl;
    cout << Solution::searchMatrix(v, 5) << " " << Solution::searchMatrix2(v, 5) << endl;
    cout << Solution::searchMatrix(v, 9) << " " << Solution::searchMatrix2(v, 9) << endl;
    cout << Solution::searchMatrix(v, 15) << " " << Solution::searchMatrix2(v, 15) << endl;
    cout << Solution::searchMatrix(v, 30) << " " << Solution::searchMatrix2(v, 30) << endl;
    cout << Solution::searchMatrix(v, 20) << " " << Solution::searchMatrix2(v, 20) << endl;
    cout << Solution::searchMatrix({{1}}, 1) << " " << Solution::searchMatrix2({{1}}, 1) << endl;
    cout << Solution::searchMatrix({{1}}, 0) << " " << Solution::searchMatrix2({{1}}, 0) << endl;
    cout << Solution::searchMatrix({{}}, 0) << " " << Solution::searchMatrix2({{}}, 0) << endl;
    cout << Solution::searchMatrix({{1, 3}}, 3) << " " << Solution::searchMatrix2({{1, 3}}, 3) << endl;

    cout << Solution::searchHelper2({1, 3, 5, 7, 10}, 1, 0, 4) << endl;
    cout << Solution::searchHelper2({1, 3, 5, 7, 10, 11}, 11, 0, 5) << endl;
    cout << Solution::searchHelper2({1, 3, 5, 7, 10}, 3, 0, 4) << endl;
    cout << Solution::searchHelper2({1, 3, 5, 7, 10}, 10, 0, 4) << endl;
    cout << Solution::searchHelper2({1, 3, 5, 7, 10}, 12, 0, 4) << endl;
}
