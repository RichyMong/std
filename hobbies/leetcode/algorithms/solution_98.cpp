// Validate Binary Search Tree - Given a binary tree, determine if it is a valid
// binary search tree (BST).
//
// Assume a BST is defined as follows:
//
// The left subtree of a node contains only nodes with keys less than the node
// s key.
// The right subtree of a node contains only nodes with keys greater
// than the node's key.
// Both the left and right subtrees must also be binary
// search trees.
//
// Example 1:
//
//     2
//    / \
//   1   3
//
// Binary tree [2,1,3], return true.
// Example 2:
//
//     1
//    / \
// /   2   3
//
// Binary tree [1,2,3], return false.
// https://leetcode.com/problems/validate-binary-search-tree/
#include "solution.h"
#include <stack>

using namespace std;

class Solution {
public:
    bool isValidBST(TreeNode* root) {
        if (root == nullptr) return true;

        vector<int> dfs;
        TreeNode *cur = root, *prev = nullptr;
        while (cur) {
            if (cur->left == nullptr) {
                dfs.push_back(cur->val);
                cur = cur->right;
            } else {
                prev = cur->left;
                while (prev->right && prev->right != cur) {
                    prev = prev->right;
                }
                if (prev->right) {
                    prev->right = nullptr;
                    dfs.push_back(cur->val);
                    cur = cur->right;
                } else {
                    prev->right = cur;
                    cur = cur->left;
                }
            }
        }

        for (int i = (int) dfs.size() - 1; i >= 1; i--) {
            if (!(dfs[i - 1] < dfs[i])) {
                return false;
            }
        }

        return true;
    }
};

int main()
{
    TreeNode left(1), right(3), root(2);
    root.left = &left;
    root.right = &right;

    cout << Solution().isValidBST(&root) << endl;

    return 0;
}
