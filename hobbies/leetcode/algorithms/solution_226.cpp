#include <iostream>
#include <vector>
#include <algorithm>
#include "solution.h"

using namespace std;

class Solution {
public:
    static TreeNode* invertTree(TreeNode* root) {
        if (!root) return NULL;
        auto left = root->left;
        root->left = invertTree(root->right);
        root->right = invertTree(root->left);
        return root;
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
    print_tree(root);
    auto v = Solution::invertTree(root);
    print_tree(root);
}
