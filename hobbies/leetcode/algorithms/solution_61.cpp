// rotate-list - Given a list, rotate the list to the right by k places, where
// k is non-negative.
// For example:
// Given 1->2->3->4->5->NULL and k = 2,
// return 4->5->1->2->3->NULL.
// https://leetcode.com/problems/rotate-list
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
    ListNode* rotateRight(ListNode* head, int k) {
        if (k <= 1 || !head || !head->next) return head;

        int size = 1;
        auto tail = head;
        while (tail->next) {
            tail = tail->next;
            ++size;
        }

        tail->next = head;
        if (k % size) {
            for (int i = 0; i < (size - k); i++) {
                tail = tail->next;
            }
        }

        head = tail->next;
        tail->next = NULL;
        return head;
    }
};

int main(int argc, char** argv)
{
    ListNode* list = create_list({1, 2, 3, 4, 5});
    print_list(Solution().rotateRight(list, argc > 1 ? stoi(argv[1]) : 3));
}
