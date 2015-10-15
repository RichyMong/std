// Container With Most Water - Given n non-negative integers a1, a2, ..., an,
// where each represents a point at coordinate (i, ai). n vertical lines are
// drawn such that the two endpoints of line i is at (i, ai) and (i, 0). Find
// two lines, which together with x-axis forms a container, such that the container
// contains the most water.
// Note: You may not slant the container.
//
// https://leetcode.com/problems/container-with-most-water/
#include "solution.h"

using namespace std;

class Solution {
public:
    static int maxArea(const vector<int>& height) {
        auto max_area = 0;
        auto i = 0, j = (int) height.size() - 1;

        while (i < j) {
            auto h = min(height[i], height[j]);
            auto area = (j  - i) * h;
            if (area > max_area) max_area = area;
            while (i < j && height[i] <= h) ++i;
            while (j > i && height[j] <= h) --j;
        }

        return max_area;
    }

};

int main()
{
    cout << Solution::maxArea({10, 20, 10, 10, 15}) << endl;
}
