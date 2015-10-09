// Insertion Sort List - Sort a linked list using insertion sort.
// https://leetcode.com/problems/insertion-sort-list/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static ListNode* insertionSortList2(ListNode* head) {
        if (head == NULL) return NULL;

        ListNode* prev = head;
        ListNode* node = head->next;

        while (node) {
            ListNode** cprev = &head;
            ListNode* pos = head;
            while (pos->val < node->val) {
                cprev = &pos->next;
                pos = pos->next;
            }

            auto next = node->next;
            if (pos == node) {
                prev = node;
            } else {
                *cprev = node;
                prev->next = node->next;
                node->next = pos;
            }
            node = next;
        }

        return head;
    }

    static ListNode* helper(ListNode* prev, ListNode* node) {
        if (prev->next == node) {
            if (prev->val < node->val)
                return prev;
            prev->next = node->next;
        } else {
            auto result = helper(prev->next, node);
            if (result) return result;
            else if (prev->val < node->val) return prev;
        }

        return NULL;
    }

    static ListNode* insertionSortList(ListNode* head) {
        ListNode* node = head->next;

        while (node) {
            auto pos = helper(head, node);
            auto next = node->next;
            if (pos) {
                if (pos->next != node) {
                    node->next = pos->next;
                    pos->next = node;
                }
            } else {
                node->next = head;
                head = node;
            }
            node = next;
            print_list(head);
        }

        return head;
    }

};

int main()
{
    auto l1 = create_list({3, 7, 1, 5, 4});
    l1 = Solution::insertionSortList2(l1);
    print_list(l1);
    cout << "-----" << endl;
    print_list(Solution::insertionSortList2(create_list({1, 2})));
    cout << "-----" << endl;
    print_list(Solution::insertionSortList2(create_list({2, 1})));
    cout << "-----" << endl;
    print_list(Solution::insertionSortList2(create_list({2, 3, 4})));
    cout << "-----" << endl;
    print_list(Solution::insertionSortList2(create_list({3, 2, 6, 4, 1})));
    cout << "-----" << endl;
    print_list(Solution::insertionSortList2(create_list({3})));
}
