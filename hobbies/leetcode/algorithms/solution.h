#ifndef LEET_CODE_SOLUTION_H
#define LEET_CODE_SOLUTION_H

#include <iostream>
#include <initializer_list>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_set>

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

template <class Iterator>
inline void pretty_print(Iterator start, Iterator end)
{
    std::cout << '[';
    while (start != end) {
        std::cout << *start;
        if (++start != end)
            std::cout << ", ";
    }
    std::cout << ']' << std::endl;
}

template <class Container>
inline void pretty_print(const Container& c)
{
    pretty_print(std::begin(c), std::end(c));
}

inline void print_list(ListNode* head)
{
    for (auto node = head; node; node = node->next) {
        std::cout << (node == head ? "" : "->") << node->val;
    }
    std::cout << "\n";
}

inline void print_tree(TreeNode* root) {
    if (!root) return;

    std::cout << root->val << std::endl;
    print_tree(root->left);
    print_tree(root->right);
}

static ListNode* create_list(const std::initializer_list<int>& nums)
{
    ListNode* head = nullptr;
    ListNode* pos = nullptr;

    for (auto x : nums) {
        if (head == nullptr) {
            head = new ListNode(x);
            pos = head;
        } else {
            pos->next = new ListNode(x);
            pos = pos->next;
        }
    }

    return head;
}

#endif
