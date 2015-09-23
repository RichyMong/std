#include <iostream>

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};

ListNode* create_list(const std::initializer_list<int>& nums)
{
    ListNode* head = nullptr;
    ListNode* pos = nullptr;

    for (auto x : nums) {
        if (head == nullptr) {
            head = new ListNode(x);
            pos = head;
        } else {
            pos->next = new ListNode(x);
            pos = pos->next;
        }
    }

    return head;
}

class Solution {
public:
    static ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
        if (!headA || !headB) return NULL;

        ListNode* p1 = headA;
        ListNode* p2 = headB;

        while (p1 && p2 && p1 != p2) {
            p1 = p1->next;
            p2 = p2->next;
            if (p1 == p2) break;

            if (p1 == NULL) p1 = headB;
            if (p2 == NULL) p2 = headA;
        }

        return p1;
    }
};

int main()
{
    auto l1 = create_list({1, 2, 3, 4, 5});
    auto l2 = create_list({7, 8, 3, 4, 5});
}
