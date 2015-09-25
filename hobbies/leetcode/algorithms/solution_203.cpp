#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static ListNode* removeElements(ListNode* head, int val) {
        ListNode** prev = &head;
        for (auto node = head; node; node = node->next) {
            if (node->val == val) {
                *prev = node->next;
            } else
                prev = &node->next;
        }

        return head;
    }

};

int main()
{
    auto l = create_list({6, 1, 2, 3, 6, 4, 5, 6});
    l = Solution::removeElements(l, 6);
    print_list(l);
    l = create_list({1, 1});
    l = Solution::removeElements(l, 1);
    print_list(l);
}
