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
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        ListNode* head = nullptr;
        for (size_t i = 0; i < lists.size(); i++) {
            for (auto node = lists[i]; node; node = node->next) {
                auto prev = &head;
                for (auto tmp = head; tmp; tmp = tmp->next) {
                    if (node->val < tmp->val) {
                        break;
                    }
                    prev = &tmp->next;
                }
                node->next = *prev;
                *prev = node;
            }
        }

        return head;
    }
};

int main()
{
    Solution::
}
