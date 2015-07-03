#include <iostream>
#include <memory>
#include <functional>

using namespace std;

void compare(int i, int j) {
    cout << i << " is " << (i < j ? "smaller" : "bigger") << " than " << j << endl;
}

int now = 4;

void check_func(int tick, function<void()> callback)
{
    if (now > tick) {
        callback();
    }
}

void test(int i)
{
    cout << __func__ << ":" << i << endl;
}

int main() {
    auto func = bind(compare, placeholders::_1, 10);
    func(1);
    func(20);

    auto f = bind(check_func, 1, bind(test, 1));
    f();
    auto g = bind(check_func, 5, bind(test, 2));
    g();
    auto h = bind(check_func, 2, bind(compare, 2, 3));
    h();
}
