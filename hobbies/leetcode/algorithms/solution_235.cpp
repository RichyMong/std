#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "solution.h"

using namespace std;

class Solution {
public:
    static TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        if (p->left == q || p->right == q) return p;
        if (q->left == p || q->right == p) return q;

        while (root) {
            if (root->val < min(p->val, q->val))
                root = root->right;
            else if (root->val > max(p->val, q->val))
                root = root->left;
            else
                break;
        }

        return root;
    }
};

int main()
{
    auto root = new TreeNode(4);
    auto left = new TreeNode(2);
    auto right = new TreeNode(6);
    left->left = new TreeNode(1);
    left->right = new TreeNode(3);
    right->left = new TreeNode(5);
    right->right = new TreeNode(7);
    root->left = left;
    root->right = right;
    print_tree(root);
    auto v = Solution::lowestCommonAncestor(root, root->left->left, root->left->right);
    cout << v->val << endl;
}
