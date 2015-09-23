#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        ListNode dummy { 0 };
        ListNode* tail = &dummy;

        while (l1 && l2) {
            if (l1->val < l2->val) {
                tail->next = l1;
                l1 = l1->next;
            } else {
                tail->next = l2;
                l2 = l2->next;
            }
            tail = tail->next;
        }

        tail->next = l1 ? l1 : l2;

        return dummy.next;
    }

};

int main()
{
    auto l1 = create_list({1, 2});
    auto l2 = create_list({3, 4, 5});
    auto l3 = create_list({1, 2, 3});
    auto l4 = create_list({2, 3, 4});
    auto r = Solution::mergeTwoLists(l1, l2);
    print_list(r);
    r = Solution::mergeTwoLists(l3, l4);
    print_list(r);
    auto l5 = create_list({6});
    auto l6 = create_list({5});
    r = Solution::mergeTwoLists(l5, l6);
    print_list(r);
}
