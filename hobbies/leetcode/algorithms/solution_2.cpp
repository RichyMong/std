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
    static int listToInt(ListNode* head) {
        if (head == nullptr) return 0;
        auto result = 0;
        while (head) { result = (result * 10) + head->val; head = head->next; }
        return result;
    }

    static ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        auto r1 = listToInt(l1);
        auto r2 = listToInt(l2);
        auto sum = r1 + r2;
        if (sum == 0) {
            return new ListNode(0);
        }
        ListNode* prev = nullptr;
        while (sum > 0) {
            ListNode* node = new ListNode(sum % 10);
            node->next = prev;
            prev = node;
            sum /= 10;
        }
        return prev;
    }

    static ListNode* addTwoNumbers2(ListNode* l1, ListNode* l2) {
        ListNode* head = nullptr;
        ListNode* pos = nullptr;

        auto sum = 0;
        while ((l1 || l2) || sum) {
            if (l1) {
                sum += l1->val;
                l1 = l1->next;
            }

            if (l2) {
                sum += l2->val;
                l2 = l2->next;
            }

            if (head == nullptr) {
                head = new ListNode(sum % 10);
                pos = head;
            } else {
                pos->next = new ListNode(sum % 10);
                pos = pos->next;
            }

            sum /= 10;
        }
        return head;
    }

};

int main()
{
    auto l1 = create_list({2, 4, 3});
    auto l2 = create_list({5, 6, 4});
    auto r = Solution::addTwoNumbers2(l1, l2);
    while (r != nullptr) {
        std::cout << r->val << " ";
        r = r->next;
    }
    auto l3 = create_list({8, 4, 3});
    auto l4 = create_list({5, 6, 4});
    r = Solution::addTwoNumbers2(l3, l4);
    while (r != nullptr) {
        std::cout << r->val << " ";
        r = r->next;
    }
    std::cout << "\n";
}
