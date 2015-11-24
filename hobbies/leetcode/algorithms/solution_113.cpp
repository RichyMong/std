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
#include <stack>
#include <deque>

using namespace std;

class Solution {
public:
    static vector<vector<int>> pathSum(TreeNode* root, int sum) {
        vector<vector<int>> result;

        if (root == NULL) return result;

        stack<TreeNode*> visting;
        deque<TreeNode*> visited;

        visting.push(root);

        auto sum_so_far = 0;

        while (!visting.empty()) {
            auto node = visting.top();
            if (!visited.empty() && visited.back() == node) {
                visited.pop_back();
                visting.pop();
                sum_so_far -= node->val;
                continue;
            }

            sum_so_far += node->val;
            if (!node->left && !node->right) {
                visting.pop();
                if (sum_so_far == sum) {
                    vector<int> nums;

                    for (auto it = visited.begin(); it != visited.end(); ++it) {
                        nums.push_back((*it)->val);
                    }

                    nums.push_back(node->val);

                    result.push_back(nums);
                }

                sum_so_far -= node->val;
            } else {
                if (node->right) visting.push(node->right);
                if (node->left) visting.push(node->left);
                visited.push_back(node);
            }
        }

        return result;
    }
};

int main()
{
    auto root = new TreeNode(5);
    auto left = new TreeNode(4);
    auto right = new TreeNode(8);
    left->left = new TreeNode(11);
    right->left = new TreeNode(9);
    right->right = new TreeNode(4);
    left->left->left = new TreeNode(7);
    left->left->right = new TreeNode(2);
    right->right->left = new TreeNode(5);
    right->right->right = new TreeNode(1);
    root->left = left;
    root->right = right;
    auto vv = Solution::pathSum(root, 22);
    for (auto& v : vv) {
        for (auto x : v) {
            cout << x << " ";
        }
        cout << endl;
    }
}
