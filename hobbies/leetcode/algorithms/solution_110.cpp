// Balanced Binary Tree - Given a binary tree, determine if it is height-balanced.

// For this problem, a height-balanced binary tree is defined as a binary tree
// in which the depth of the two subtrees of every node never differ by more than 1.

// https://leetcode.com/problems/balanced-binary-tree/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int height(TreeNode* root) {
        if (root == NULL) return 0;
        return max(height(root->left), height(root->right)) + 1;
    }

    static bool isBalanced(TreeNode* root) {
        if (root == NULL)
            return true;
        return abs(height(root->left) - height(root->right)) <= 1
               && isBalanced(root->left) && isBalanced(root->right);
    }
};

int main()
{
    auto root = new TreeNode(4);
    auto left = new TreeNode(2);
    auto right = new TreeNode(7);
    left->left = new TreeNode(1);
    left->right = new TreeNode(3);
    right->left = new TreeNode(6);
    right->right = new TreeNode(9);
    root->left = left;
    root->right = right;
    cout << Solution::isBalanced(root) << endl;
}
