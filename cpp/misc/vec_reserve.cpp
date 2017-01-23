#include <iostream>
#include <vector>

using namespace std;

int main()
{
    std::vector<int> vi {1, 2, 3, 4, 5};
    auto rb = vi.rbegin();
    cout << "rbegin=" << *rb << ", rbegin.base()=" << *rb.base() << endl;
    auto re = vi.rend();
    cout << "rend=" << *re << ", rend.base()=" << *re.base() << endl;

    return 0;
}
