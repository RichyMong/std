// Remove Duplicates from Sorted List
// Given a sorted linked list, delete all duplicates such that each element
// appear only once.
// For example,
// Given 1->1->2, return 1->2.
// Given 1->1->2->3->3, return 1->2->3.

// https://leetcode.com/problems/remove-duplicates-from-sorted-list/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static ListNode* deleteDuplicates(ListNode* head) {
        if (head == NULL) return head;

        ListNode* prev = head;
        for (ListNode* node = head->next; node; node = node->next) {
            if (node->val == prev->val) {
                prev->next = node->next;
            } else {
                prev = node;
            }
        }

        return head;
    }

    static ListNode* deleteDuplicates2(ListNode* head) {
    }
};

int main()
{
    auto head1 = create_list({1,2,3});
    auto head2 = create_list({1,1,2});
    auto head3 = create_list({1,1,2,3,3});
    print_list(Solution::deleteDuplicates(head1));
    print_list(Solution::deleteDuplicates(head2));
    print_list(Solution::deleteDuplicates(head3));
}
