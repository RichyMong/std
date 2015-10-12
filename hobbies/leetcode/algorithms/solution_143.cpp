// Reorder List
// Given a singly linked list L: L0L1Ln-1Ln,
// reorder it to: L0LnL1Ln-1L2Ln-2
// You must do this in-place without altering the nodes' values.
//
// For example,
// Given {1,2,3,4}, reorder it to {1,4,2,3}.
//
// https://leetcode.com/problems/reorder-list/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static void reorderList(ListNode* head) {
        ListNode* slow = head;
        ListNode* fast = head;

        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        ListNode* tail = NULL;
        for (auto node = slow->next; node; ) {
            auto tmp = node->next;
            node->next = tail;
            tail = node;
            node = tmp;
        }

        slow->next = NULL;
        while (head && tail) {
            auto tmp = tail->next;
            tail->next = head->next;
            head->next = tail;
            head = tail->next;
            tail = tmp;
        }
    }
};

int main()
{
    auto head = create_list({1, 2, 3, 4});
    Solution::reorderList(head);
    print_list(head);

    head = create_list({1});
    Solution::reorderList(head);
    print_list(head);

    head = create_list({1, 2});
    Solution::reorderList(head);
    print_list(head);

    head = create_list({1, 2, 3});
    Solution::reorderList(head);
    print_list(head);

    head = create_list({1, 2, 3, 4, 5});
    Solution::reorderList(head);
    print_list(head);
}
