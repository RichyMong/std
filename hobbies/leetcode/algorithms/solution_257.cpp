#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "solution.h"

using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

class Solution {
public:
    static vector<string> binaryTreePaths(TreeNode* root) {
        if (!root) return vector<string> { };

        if (!root->left && !root->right) {
            return vector<string> { to_string(root->val) };
        }

        vector<string> vs;

        auto leading = to_string(root->val) + "->";
        auto func = [&](const string& s){ vs.push_back(leading + s); };

        auto ls = binaryTreePaths(root->left);
        for_each(ls.begin(), ls.end(), func);

        auto rs = binaryTreePaths(root->right);
        for_each(rs.begin(), rs.end(), func);

        return vs;
    }

};

int main()
{
    auto root = new TreeNode(1);
    auto left = new TreeNode(2);
    auto right = new TreeNode(3);
    left->right = new TreeNode(5);
    left->left = new TreeNode(4);
    right->left = new TreeNode(6);
    right->right = new TreeNode(7);
    root->left = left;
    root->right = right;
    auto v = Solution::binaryTreePaths(root);
    pretty_print(v);
}
