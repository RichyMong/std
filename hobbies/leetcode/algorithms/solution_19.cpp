#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static ListNode* removeNthFromEnd(ListNode* head, int n) {
        if (!head || !head->next) return NULL;

        auto fast = head;
        for (int i = 0; i < n; ++i)
            fast = fast->next;

        if (fast == NULL) return head->next;

        auto slow = head;
        while (fast->next) {
            slow = slow->next;
            fast = fast->next;
        }

        slow->next = slow->next->next;

        return head;
    }

    static ListNode* removeNthFromEnd2(ListNode* head, int n) {
        ListNode** slow = &head;
        ListNode* fast = head;
        for (int i = 1; i < n; ++i)
            fast = fast->next;

        while (fast->next) {
            slow = &((*slow)->next);
            fast = fast->next;
        }

        *slow = (*slow)->next;

        return head;
    }

};

int main()
{
    auto head = create_list({1, 2, 3, 4, 5});
    head = Solution::removeNthFromEnd(head, 2);
    print_list(head);
    head = Solution::removeNthFromEnd(head, 1);
    print_list(head);
    head = create_list({1, 2});
    head = Solution::removeNthFromEnd(head, 2);
    print_list(head);
}
