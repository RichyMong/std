#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

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

void print_tree(TreeNode* root) {
    if (!root) return;

    cout << root->val << endl;
    print_tree(root->left);
    print_tree(root->right);
}

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
