// combination-sum - Given a set of candidate numbers (C) and a target number
// (T), find all unique combinations in C where the candidate numbers sums to T.
//
// The same repeated number may be chosen from C unlimited number of times
//
// Note:
// All numbers (including target) will be positive integers.
//
// The solution set must not contain duplicate combinations.
//
//
// For example, given candidate set [2, 3, 6, 7] and target 7,
// A solution set
// is:
//
// [
//   [7],
//   [2, 2, 3]
// ]
// https://leetcode.com/problems/combination-sum
#include "solution.h"

using namespace std;

class Solution {
    void combine(const vector<int>& cand, int idx, int target, vector<int> &path, vector<vector<int>>&res){
        for( ;idx<cand.size() && cand[idx] <= target ; ++idx){
            path.push_back(cand[idx]);
            if(cand[idx] == target) res.push_back(path);
            else combine(cand, idx, target - cand[idx], path, res);
            path.pop_back();
        }
    }
public:
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        sort(candidates.begin(), candidates.end());
        vector<vector<int>> res;
        vector<int> path;
        combine(candidates, 0, target, path, res);
        return res;
    }
};

int main()
{
    vector<int> vi {1, 2};
    Solution().combinationSum(vi, 4);
}
