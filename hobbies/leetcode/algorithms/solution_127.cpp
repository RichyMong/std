// Word Ladder
// Given two words (beginWord and endWord), and a dictionary's word list, find
// the length of shortest transformation sequence from beginWord to endWord,
// such that:
//
// Only one letter can be changed at a time
// Each intermediate word must exist in the word list
//
// For example,
//
// Given:
// beginWord = "hit"
// endWord = "cog"
// wordList = ["hot","dot","dog","lot","log"]
//
// As one shortest transformation is "hit" -> "hot" -> "dot" -> "dog" -> "cog",
// return its length 5.
//
// Note:
// Return 0 if there is no such transformation sequence.
// All words have the same length.
// All words contain only lowercase alphabetic characters.
//
// https://leetcode.com/problems/word-ladder/
#include <queue>
#include "solution.h"

using namespace std;

class Solution {
public:
    static int ladderLength(string beginWord, string endWord, unordered_set<string>& wordList) {
        if (wordList.size() == 0) return 0;

        queue<string> qs;
        qs.push(beginWord);
        wordList.erase(beginWord);

        int steps = 1, n = beginWord.size();

        while (!qs.empty()) {
            int count = qs.size();

            for (int i = 0; i < count; ++i) {
                auto s = qs.front();
                qs.pop();
                for (int j = 0; j < n; ++j) {
                    for (char c = 'a'; c <= 'z'; ++c) {
                        if (s[j] == c) continue;
                        swap(s[j], c);
                        if (s == endWord) return steps + 1;
                        if (wordList.find(s) != wordList.end()) {
                            qs.push(s);
                            wordList.erase(s);
                        }
                        swap(s[j], c);
                    }
                }
            }

            ++steps;
        }

        return 0;
    }

};

int main()
{
    unordered_set<string> dict { "hot","dot","dog","lot","log", "cot" };
    cout << Solution::ladderLength("hit", "cog", dict) << endl;
    unordered_set<string> abc { "a", "b", "c"};
    cout << Solution::ladderLength("a", "c", abc) << endl;
}
