#include <iostream>

using namespace std;

class Solution {
public:
    int computeArea(int A, int B, int C, int D, int E, int F, int G, int H) {
        auto left = max(A, E);
        auto right = max(min(C, G), left);
        auto bottom = max(B, F);
        auto top = max(min(D, H), bottom);
        return (C - A) * (D - B) + (G - E) * (H - F) - (right - left) * (top - bottom);
    }
};

int main()
{
}
