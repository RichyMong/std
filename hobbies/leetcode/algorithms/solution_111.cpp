// Minimum Depth of Binary Tree - Given a binary tree, find its minimum depth.
// The minimum depth is the number of nodes along the shortest path from the
// root node down to the nearest leaf node.
// https://leetcode.com/problems/minimum-depth-of-binary-tree/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int minDepth(TreeNode* root) {
        if (root == NULL || (root->left == NULL && root->right == NULL)) {
            return 1;
        }

        if (root->left && root->right)
            return min(minDepth(root->left), minDepth(root->right)) + 1;
        else if (root->left)
            return minDepth(root->left) + 1;
        else
            return minDepth(root->right) + 1;
    }
};

int main()
{
}
