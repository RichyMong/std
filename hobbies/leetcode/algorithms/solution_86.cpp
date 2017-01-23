// partition-list - Given a linked list and a value x, partition it such that
// all nodes less than x come before nodes greater than or equal to x.
//
// You should preserve the original relative order of the nodes in each of the
// two partitions.
//
// For example,
// Given 1->4->3->2->5->2 and x = 3,
// // return 1->2->2->4->3->5.
// https://leetcode.com/problems/partition-list
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
    ListNode* partition(ListNode* head, int x) {
        ListNode first { 0 }, second { 0 };
        ListNode** middle = &first.next;
        ListNode** end = &second.next;
        ListNode* node = head;
        while (node) {
            auto tmp = node;
            node = node->next;
            if (tmp->val < x) {
                *middle = tmp;
                middle = &tmp->next;
            } else {
                *end = tmp;
                end = &tmp->next;
            }
        }
        *middle = second.next;
        *end = NULL;
        return first.next;
    }
};

int main()
{
    auto l1 = create_list({5, 6, 3, 4, 2, 2, 1, 7});
    Solution s;
    print_list(s.partition(l1, 3));
    auto l2 = create_list({5});
    print_list(s.partition(l2, 3));
    auto l3 = create_list({1,2});
    print_list(s.partition(l3, 3));
    auto l4 = create_list({5,6});
    print_list(s.partition(l4, 3));
}
