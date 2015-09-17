#include <iostream>
#include <cstdint>

class Solution {
public:
    static int hammingWeight(uint32_t n) {
        n = ((n >> 1) & 0x55555555) + (n & 0x55555555);
        n = ((n >> 2) & 0x33333333) + (n & 0x33333333);
        n = ((n >> 4) & 0x0F0F0F0F) + (n & 0x0F0F0F0F);
        n = ((n >> 8) & 0x00FF00FF) + (n & 0x00FF00FF);
        n = ((n >> 16) & 0xFFFF) + (n & 0xFFFF);
        return n;
    }
};

int main()
{
    std::cout << Solution::hammingWeight(0xF7) << std::endl;
    std::cout << Solution::hammingWeight(0x00) << std::endl;
    std::cout << Solution::hammingWeight(0xFF) << std::endl;
    std::cout << Solution::hammingWeight(0xFFFFFFFF) << std::endl;
}
