#include <stack>
#include <queue>
#include "solution.h"

using namespace std;

void dfs(TreeNode* root, int val)
{
    cout << __func__ << ": " << val << endl;
    stack<TreeNode*> stk;

    stk.push(root);

    while (!stk.empty()) {
        auto node = stk.top();
        stk.pop();

        cout << "visiting " << node->val << endl;
        if (node->val == val) break;
        if (node->right) stk.push(node->right);
        if (node->left) stk.push(node->left);
    }
}

void bfs(TreeNode* root, int val)
{
    cout << __func__ << ": " << val << endl;
    queue<TreeNode*> q;

    q.push(root);

    while (!q.empty()) {
        auto node = q.front();
        q.pop();

        cout << "visiting " << node->val << endl;
        if (node->val == val) break;
        if (node->left) q.push(node->left);
        if (node->right) q.push(node->right);
    }

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
    dfs(root, 3);
    dfs(root, 9);
    bfs(root, 3);
    bfs(root, 9);
}
