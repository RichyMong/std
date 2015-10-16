// Letter Combinations of a Phone Number - Given a digit string, return all pos-
// sible letter combinations that the number could represent.
//
// A mapping of
// digit to letters (just like on the telephone buttons) is given below.
//
// Input:Digit string "23"
// Output: ["ad", "ae", "af", "bd", "be", "bf", cd", "ce", "cf"].
//
// Note:
// Although the above answer is in lexicographical order, your answer could
// be in any order you want.
//
// https://leetcode.com/problems/letter-combinations-of-a-phone-number/
#include "solution.h"

using namespace std;

class Solution {
public:
    static vector<string> letterCombinations(string digits) {
        if (digits.empty() || digits.find_first_not_of("23456789") != string::npos) return {};

        vector<string> table { "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};

        auto after = letterCombinations(digits.substr(1));

        vector<string> result;

        for (auto x : table[digits[0] - '2']) {
            if (after.empty()) {
                result.push_back(string(1, x));
            } else {
                for (auto& s : after) {
                    result.push_back(x + s);
                }
            }
        }

        return result;
    }

    static vector<string> letterCombinations2(string digits) {
        vector<string> table { "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};

        vector<string> result;

        for (auto x : digits) {
            if (x < '2' || x > '9')
                break;
            vector<string> tmp;
            for (auto y : table[x - '2']) {
                if (!result.empty()) {
                    for (auto& s : result) {
                        tmp.emplace_back(s + y);
                    }
                } else {
                    tmp.emplace_back(1, y);
                }
            }

            result = std::move(tmp);
        }

        return result;
    }
};

int main()
{
    auto result = Solution::letterCombinations("28");
    for (auto& x : result) {
        cout << x << endl;
    }
    result = Solution::letterCombinations("20");
}
