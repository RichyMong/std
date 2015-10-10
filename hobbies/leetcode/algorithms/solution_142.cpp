// Linked List Cycle II
// Given a linked list, return the node where the cycle begins. If there is no cycle, return null.
//
// Note: Do not modify the linked list.
// Follow up:
// Can you solve it without using extra space?
//
// https://leetcode.com/problems/linked-list-cycle-ii/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static ListNode *detectCycle(ListNode *head) {
        ListNode* prev = head;
        ListNode* slow = head;
        ListNode* fast = head;

        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            if (slow == fast) {
                while (prev != slow) {
                    slow = slow->next;
                    prev = prev->next;
                }
                return prev;
            }
        }

        return NULL;
    }

};

int main()
{
    auto head = new ListNode(10);
    head->next = head;
    auto node = Solution::detectCycle(head);
    cout << node->val << endl;
}
