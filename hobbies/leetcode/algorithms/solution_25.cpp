// Reverse Nodes in k-Group
// https://leetcode.com/problems/reverse-nodes-in-k-group/
#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static ListNode* reverseKGroup(ListNode* head, int k) {
        if (k <= 0) return head;

        ListNode** prev = &head;
        vector<ListNode*> group(k);

        ListNode *node = head;
        while (node) {
            int i = 0;

            for (i = 0; i < k && node; ++i) {
                group[i] = node;
                node = node->next;
            }

            if (i < k) continue;

            auto end = group.back();
            group[0]->next = end->next;
            *prev = end;
            prev = &group[0]->next;
            node = group[0]->next;

            for (int j = (int) group.size() - 1; j > 0; --j) {
                group[j]->next = group[j - 1];
            }
        }

        return head;
    }

    static ListNode* reverseKGroup2(ListNode* head, int k) {
        if (k <= 1) return head;

        int length = 0;

        for (auto node = head; node; node = node->next) {
            ++length;
        }

        ListNode** last = &head;
        ListNode* node = head;
        for (auto pos = 0; pos + k <= length; pos += k) {
            ListNode* prev = node;
            ListNode* current = node;
            node = node->next;
            for (auto i = 1; i < k; ++i) {
                auto next = node->next;
                node->next = prev;
                prev = node;
                node = next;
            }

            current->next = node;
            *last = prev;
            last  = &current->next;
        }

        return head;
    }

};

int main()
{
    print_list(Solution::reverseKGroup2(create_list({1, 2, 3, 4}), 2));
    print_list(Solution::reverseKGroup2(create_list({1, 2, 3, 4, 5}), 2));
    print_list(Solution::reverseKGroup2(create_list({1, 2, 3, 4, 5}), 3));
}
