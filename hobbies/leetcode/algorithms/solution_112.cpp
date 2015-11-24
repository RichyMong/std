// Path Sum - Given a binary tree and a sum, determine if the tree has a
// root-to-leaf path such that adding up all the values along the path equals
// the given sum.
//
// For example:
// Given the below binary tree and sum = 22,
//
//               5
//              / \
//             4   8
//            /   / \
//           11  13  4
//          /  \      \
//         7    2      1
//
// return true, as there exist a root-to
// leaf path 5->4->11->2 which sum is 22.
// https://leetcode.com/problems/path-sum/
#include "solution.h"
#include <stack>

using namespace std;

class Solution {
public:
    static bool hasPathSum(TreeNode* root, int sum) {
        if (root == NULL) return false;

        stack<TreeNode*> visting;
        unordered_set<TreeNode*> visited;

        visting.push(root);

        auto sum_so_far = 0;

        while (!visting.empty()) {
            auto node = visting.top();
            auto it = visited.find(node);
            if (it != visited.end()) {
                visited.erase(it);
                visting.pop();
                sum_so_far -= node->val;
                continue;
            }

            sum_so_far += node->val;
            if (!node->left && !node->right) {
                visting.pop();
                if (sum_so_far == sum) return true;
                else sum_so_far -= node->val;
            } else {
                if (node->right) visting.push(node->right);
                if (node->left) visting.push(node->left);
                visited.insert(node);
            }
        }

        return false;
    }

    static bool hasPathSum2(TreeNode* root, int sum) {
        if (root == NULL) return false;

        if (root->left == NULL && root->right == NULL)
            return root->val == sum;

        return hasPathSum2(root->left, sum - root->val) ||
               hasPathSum2(root->right, sum - root->val);
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
    right->right->right = new TreeNode(1);
    root->left = left;
    root->right = right;

    cout << Solution::hasPathSum(root, 22) << endl;
    cout << Solution::hasPathSum2(root, 22) << endl;
}
