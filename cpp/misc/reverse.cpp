#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main(int argc, const char *argv[])
{
    std::vector<int> vi { 1, 2, 2, 3, 5, 5 };

    cout << *vi.rbegin() << endl;

    for (auto it = vi.rbegin();  it != vi.rend(); ) {
        auto start = lower_bound(vi.begin(), it.base(), *it);
        auto tmp = start;
        while (start != it.base()) {
            std::cout << *start << ",";
            ++start;
        }

        std::cout << std::endl;
        it = std::vector<int>::reverse_iterator(tmp);
    }

    for (int i = (int) vi.size() - 1; i >= 0; ) {
        int j = i - 1;
        while (j >=0 && vi[j] == vi[i])
            --j;
        for (int k = j + 1; k <= i; k++) {
            std::cout << vi[k] << ' ';
        }
        std::cout << std::endl;
        i = j;
    }

    return 0;
}
