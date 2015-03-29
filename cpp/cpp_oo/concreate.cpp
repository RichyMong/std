#include <iostream>

using namespace std;

class X {
    int x;
    char c1;
};

class Y : public X {
    char c2;
};

class Z : public Y {
    char c3;
};

int main() {
    cout << "sizeof(X): " << sizeof(X) << endl;
    cout << "sizeof(Y): " << sizeof(Y) << endl;
    cout << "sizeof(Z): " << sizeof(Z) << endl;
}
