#include <iostream>
#include "solution.h"

using namespace std;

class Solution {
public:
    static bool isPalindrome(ListNode* head) {
        if (head == NULL) return true;

        int n = 0;
        for (ListNode* node = head; node; node = node->next)
            ++n;

        ListNode* node = head;
        ListNode* prev = head;
        for (int i = 0; i < (n + 1) / 2; ++i) {
            prev = node;
            node = node->next;
        }

        ListNode* tail = NULL;
        while (node) {
            auto temp = node->next;
            node->next = tail;
            tail = node;
            node = temp;
        }

        prev->next = tail;

        while (tail && tail->val == head->val) {
            tail = tail->next;
            head = head->next;
        }

        return tail == NULL;
    }

    static bool isPalindrome2(ListNode* head) {
        if (head == NULL) return true;

        ListNode* slow = head;
        ListNode* fast = head;
        while (fast->next && fast->next->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        ListNode* tail = NULL;
        auto node = slow->next;

        while (node) {
            auto temp = node->next;
            node->next = tail;
            tail = node;
            node = temp;
        }

        slow->next = tail;
        while (tail) {
            if (tail->val != head->val) return false;
            tail = tail->next;
            head = head->next;
        }

        return true;
    }
};

int main()
{
    auto l1 = create_list({1, 2, 1});
    auto l2 = create_list({1, 2, 2, 1});
    auto l3 = create_list({1, 2, 2, 3});
    auto l4 = create_list({1});
    auto l5 = create_list({1, 0, 0});
    cout << Solution::isPalindrome(l1) << endl;
    cout << Solution::isPalindrome(l2) << endl;
    cout << Solution::isPalindrome(l3) << endl;
    cout << Solution::isPalindrome(l4) << endl;
    cout << Solution::isPalindrome(l5) << endl;
    // cout << Solution::isPalindrome2(l1) << endl;
    // cout << Solution::isPalindrome2(l2) << endl;
    // cout << Solution::isPalindrome2(l3) << endl;
    // cout << Solution::isPalindrome2(l4) << endl;
    // cout << Solution::isPalindrome2(l5) << endl;
}
