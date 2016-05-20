#include <iostream>
#include <type_traits>
#include "util.h"

using namespace std;

void test()
{
    std::cout << "test() called" << std::endl;
}

int fetch()
{
    cout << "Enter a number: ";
    cout.flush();
    int n;
    cin >> n;
    return n;
}

struct S {
    int operator()(int) { cout << "S(int) called\n"; return 10; }
    void operator()() { cout << "S()  called\n"; }
};

int main()
{
    using namespace std;

    auto s =  S();
    util::timeit(s, 1);
    auto t = util::timeit(s);
    util::timeit(test);
    cout << "with time: "  << t.count() << endl;
    auto p = util::timeit(fetch);
    cout << p.first << ":" << p.second.count() << endl;
}
