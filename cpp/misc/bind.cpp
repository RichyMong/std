#include <iostream>
#include <memory>

using namespace std;

void compare(int i, int j) {
    cout << i << " is " << (i < j ? "smaller" : "bigger") << " than " << j << endl;
}

int main() {
    auto func = bind(compare, placeholders::_1, 10);
    func(1);
    func(20);
}
