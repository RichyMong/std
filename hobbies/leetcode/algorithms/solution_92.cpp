// reverse-linked-list-ii - Reverse a linked list from position m to n. Do it
// in-place and in one-pass.
//
// For example:
// Given 1->2->3->4->5->NULL, m = 2 and n = 4,
// return 1->4->3->2->5->NULL.
//
// Note:
// Given m, n satisfy the following condition:
// 1 ≤ m ≤ n ≤ length of list.
// https://leetcode.com/problems/reverse-linked-list-ii
#include "solution.h"

using namespace std;

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    ListNode* reverseBetween(ListNode* head, int m, int n) {
        int pos = 1;
        ListNode** prev = &head;
        ListNode* cur = head;
        while (cur && pos < m) {
            prev = &cur->next;
            cur = cur->next;
            ++pos;
        }

        if (!cur) return head;

        ListNode* pos_node = cur;
        ListNode* tail = cur;
        while ((cur = tail->next) && pos < n) {
            tail->next = cur->next;
            cur->next = pos_node;
            pos_node = cur;
            ++pos;
        }
        *prev = pos_node;
        return head;
    }
};

int main()
{
    Solution s;
    auto l2 = create_list({1,2,3,4,5});
    print_list(l2);
    print_list(s.reverseBetween(l2, 2, 4));
    print_list(s.reverseBetween(l2, 6, 5));
}
