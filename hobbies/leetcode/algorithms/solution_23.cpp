// merge-k-sorted-lists - Merge k sorted linked lists and return it as one sorted
// list. Analyze and describe its complexity.
// https://leetcode.com/problems/merge-k-sorted-lists
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
    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
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

    ListNode* mergeKLists(ListNode** beg, int size)
    {
        if (size < 2) {
            return *beg;
        } else if (size == 2) {
            return mergeTwoLists(beg[0], beg[1]);
        } else {
            auto median = size / 2;
            auto first = mergeKLists(beg, median);
            auto second = mergeKLists(beg + median, size - median);
            return mergeTwoLists(first, second);
        }
    }

    ListNode* mergeKLists(vector<ListNode*>& lists) {
        if (lists.empty()) return nullptr;

        return mergeKLists(&*lists.begin(), lists.size());
    }
};

int main()
{
    Solution s;
    {

        auto l1 = create_list({1, 2});
        auto l2 = create_list({3, 4, 5});
        auto l3 = create_list({1, 2, 3});
        vector<ListNode*> lists_3 { l1, l2, l3 };
        auto r3 = s.mergeKLists(lists_3);
        print_list(r3);
    }

    {
        auto l1 = create_list({1, 2});
        auto l2 = create_list({3, 4, 5});
        auto l3 = create_list({1, 2, 3});
        auto l4 = create_list({2, 3, 4});
        vector<ListNode*> lists_4 { l1, l2, l3, l4};
        auto r4 = s.mergeKLists(lists_4);
        print_list(r4);
    }
}

