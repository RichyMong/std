// Path Sum II - Given a binary tree and a sum, find all root-to-leaf paths where
// each path's sum equals the given sum.
//
//
// For example:
// Given
// the below binary tree and sum = 22,
//
//               5
//              / \
//             4   8
//            /   / \
//           11  13  4
//           / \    / \
//          7   2  5   1
//
// return
//
// [
//    [5,4,11,2],
//    [5,8,4,5]
// ]
// https://leetcode.com/problems/path-sum-ii/
#include "solution.h"

using namespace std;

class Solution {
public:
    static vector<vector<int>> pathSum(TreeNode* root, int sum) {
    }
};

int main()
{
    auto root = new TreeNode(5);
    auto left = new TreeNode(4);
    auto right = new TreeNode(8);
    left->left = new TreeNode(11);
    right->left = new TreeNode(13);
    right->right = new TreeNode(4);
    left->left->left = new TreeNode(7);
    left->left->right = new TreeNode(2);
    right->right->left = new TreeNode(5);
    right->right->right = new TreeNode(1);
    root->left = left;
    root->right = right;
    auto v = Solution::pathSum(root, 22);
}
