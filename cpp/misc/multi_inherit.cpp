#include <iostream>

using namespace std;

class A {
private:
    int x, y;
};

class B {
private:
    char b;
};

class C : public A, public B {
public:
    char c;
};

class VA {
private:
    virtual void fa() { }
};

class VB {
private:
    virtual void fb() { }
};

class VC : public VA, public VB {
public:
    virtual void fc() { }
};

class V_A : public A, public VA {
};

int main() {
    C c;
    A* pa = &c;
    B* pb = &c;

    cout << pa << endl;
    cout << pb << endl;
    cout << static_cast<void*>(&c.c) << endl;

    VC vc;
    VA* pva = &vc;
    VB* pvb = &vc;
    cout << pva << endl;
    cout << pvb << endl;
    cout << static_cast<void*>(&vc) << endl;

    V_A va;
    pva = &va;
    pa = &va;
    cout << pa << endl;
    cout << pva << endl;
    cout << static_cast<void*>(&va) << endl;
}
