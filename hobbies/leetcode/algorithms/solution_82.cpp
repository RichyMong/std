// Remove Duplicates from Sorted List II
// Given a sorted linked list, delete all nodes that have duplicate numbers, leaving only distinct numbers from the original list.
//
// For example,
// Given 1->2->3->3->4->4->5, return 1->2->5.
// Given 1->1->1->2->3, return 2->3.
//
// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static ListNode* deleteDuplicates(ListNode* head) {
        if (head == NULL || head->next == NULL)
            return head;

        ListNode** prev = &head;

        for (ListNode* node = head; node; ) {
            if (node->next && node->next->val == node->val) {
                auto ref = node;
                while (node && node->val == ref->val) {
                    node = node->next;
                }

                *prev = node;
            } else {
                prev = &node->next;
                node = node->next;
            }
        }

        return head;
    }
};

int main()
{
    print_list(Solution::deleteDuplicates(create_list({1, 2, 3})));
    print_list(Solution::deleteDuplicates(create_list({1, 1, 1, 2, 3})));
    print_list(Solution::deleteDuplicates(create_list({1, 2, 3, 3})));
    print_list(Solution::deleteDuplicates(create_list({1, 2, 3, 3, 4, 5, 6, 6})));
    print_list(Solution::deleteDuplicates(create_list({1, 2, 3, 3, 4, 4, 5})));
    print_list(Solution::deleteDuplicates(create_list({1})));
    print_list(Solution::deleteDuplicates(create_list({2, 2})));
}
