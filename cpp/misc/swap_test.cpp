#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Test {
public:
    Test() { cout << "construct Test\n"; }
    Test(const Test&) { cout << "copy Test\n"; }
};

void test()
{
    map<int, vector<Test>> tests;
    vector<Test> ab { Test(), Test(), Test() };
    tests[0] = ab;
    cout << "expected no copy\n";
    tests[1].swap(ab);
}

int main()
{
    test();
    return 0;
}
