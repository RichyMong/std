#include <iostream>

using namespace std;

class Interface {
public:
    virtual void run(int i = 10) = 0;
};

class Impl : public Interface {
public:
    void run(int i = 20) { cout << i << endl; }
};

int main()
{
    Impl impl;
    impl.run();
    Interface& intf = impl;
    intf.run();
    return 0;
}
