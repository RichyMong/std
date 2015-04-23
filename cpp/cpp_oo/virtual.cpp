#include <iostream>

using namespace std;

class X {
public:
    virtual void foo() { }
    int x;
    char c1;
};

class Y : public X {
public:
    char c2;
};

class Z : public Y {
public:
    char c3;
};

int main() {
    char X::*pcx = &X::c1;

    cout << "sizeof(X): " << sizeof(X) << endl;
    X x;
    cout << "addressof(x): " << &x << ", member: " << &x.x << ", " << static_cast<void*>(&x.c1) << endl;
    char Y::*pcy = &Y::c2;
    Y y;
    cout << "sizeof(Y): " << sizeof(Y) << endl;
    cout << "addressof(y): " << &y << ", member: " << static_cast<void*>(&y.c2) << endl;
    char Z::*pcz = &Z::c3;
    Z z;
    cout << "sizeof(Z): " << sizeof(Z) << endl;
    cout << "addressof(z): " << &z << ", member: " << static_cast<void*>(&z.c3) << endl;
}
