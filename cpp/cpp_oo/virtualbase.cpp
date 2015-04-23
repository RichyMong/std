#include <iostream>

class Base {
public:
    virtual ~Base() = default;
    virtual void draw() { }
protected:
    int x;
};

class Derived1 : public virtual Base {
public:
    virtual void koke() { }
};

class Derived2 : public virtual Base {
public:
    virtual void joke() { }
};

class Derived3 : public Derived1, public Derived2 {
};

int main() {
    using namespace std;
    cout << "sizeof(Base): " << sizeof(Base) << endl;
    cout << "sizeof(Derived1): " << sizeof(Derived1) << endl;
    cout << "sizeof(Derived2): " << sizeof(Derived2) << endl;
    cout << "sizeof(Derived3): " << sizeof(Derived3) << endl;
    return 0;
}
