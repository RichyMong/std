// Linked List Cycle
// Given a linked list, determine if it has a cycle in it.
//
// Follow up:
// Can you solve it without using extra space?
//
// https://leetcode.com/problems/linked-list-cycle/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static bool hasCycle(ListNode *head) {
        ListNode* slow = head;
        ListNode* fast = head;

        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            if (slow == fast)
                return true;
        }

        return false;
    }

};

int main()
{
    auto head = new ListNode(10);
    cout << Solution::hasCycle(head) << endl;
    head->next = head;
    cout << Solution::hasCycle(head) << endl;
    head->next = new ListNode(20);
    cout << Solution::hasCycle(head) << endl;
    auto h1 = create_list({3, 2, 0, -4});
    cout << Solution::hasCycle(h1) << endl;
}
