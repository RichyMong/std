// Longest Substring Without Repeating Characters Given a string, find the length
// of the longest substring without repeating characters. For example, the longest
// substring without repeating letters for "abcabcbb" is "abc", which the length
// is 3. For "bbbbb" the longest substring is "b", with the length of 1.
// https://leetcode.com/problems/longest-substring-without-repeating-characters/
#include "solution.h"

using namespace std;

class Solution {
public:
    static int lengthOfLongestSubstring(const string& s) {
        unordered_map<char, int> um;

        int longest = 0;
        int start = -1;

        for (int i = 0; i < s.size(); ++i) {
            auto it = um.find(s[i]);
            if (it != um.end()) {
                start = max(it->second, start);
            }
            longest = max(longest, i - start);
            um[s[i]] = i;
        }

        return longest;
    }
};

int main()
{
    cout << Solution::lengthOfLongestSubstring("abcabcdb") << endl;
    cout << Solution::lengthOfLongestSubstring("bbbbb") << endl;
    cout << Solution::lengthOfLongestSubstring("c") << endl;
    cout << Solution::lengthOfLongestSubstring("abc") << endl;
    cout << Solution::lengthOfLongestSubstring("aab") << endl;
    cout << Solution::lengthOfLongestSubstring("cdd") << endl;
    cout << Solution::lengthOfLongestSubstring("abba") << endl;
    cout << Solution::lengthOfLongestSubstring("ggububgvfk") << endl;
}
