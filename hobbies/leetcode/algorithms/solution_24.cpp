// Swap Nodes in Pairs -
// Given a linked list, swap every two adjacent nodes
// and return its head.
//
// For example,
// Given 1->2->3->4, you should
// return the list as 2->1->4->3.
//
// Your algorithm should use only constant
// space. You may not modify the values in the list, only nodes itself can be
// changed.
//
// https://leetcode.com/problems/swap-nodes-in-pairs/
#include "solution.h"

using namespace std;

class Solution {
public:
    static ListNode* swapPairs(ListNode* head) {
        auto slow = head;
        auto prev = &head;

        while (slow && slow->next) {
            auto fast = slow->next;
            auto next = fast->next;
            *prev = fast;
            fast->next = slow;
            slow->next = next;
            prev = &slow->next;
            slow = next;
        }

        return head;
    }
};

int main()
{
    print_list(Solution::swapPairs(create_list({1})));
    print_list(Solution::swapPairs(create_list({1, 2})));
    print_list(Solution::swapPairs(create_list({1, 2, 3})));
    print_list(Solution::swapPairs(create_list({1, 2, 3, 4})));
    print_list(Solution::swapPairs(create_list({1, 2, 3, 4, 5})));
}
