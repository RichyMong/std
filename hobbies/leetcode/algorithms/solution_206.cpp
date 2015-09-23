#include <iostream>
#include "solution.h"

class Solution {
public:
    static ListNode* reverseList(ListNode* head) {
        ListNode* prev = NULL;
        ListNode* node = head;

        while (node) {
            auto tmp = node->next;
            node->next = prev;
            prev = node;
            node = tmp;
        }

        return prev;
    }
};

int main()
{
    auto l1 = create_list({2, 4, 3});
    print_list(l1);
    auto r = Solution::reverseList(l1);
    print_list(r);

    auto l2 = create_list({5, 6, 4});
    print_list(l2);
    auto r2 = Solution::reverseList(l2);
    print_list(r2);
    auto l3 = create_list({8});
    r = Solution::reverseList(l3);
    print_list(r);
}
