#include <iostream>

using namespace std;

class PubX {
public:
    int x;
    char c1;
};

class PubY : public PubX {
public:
    char c2;
};

class PubZ : public PubY {
public:
    char c3;
};

class PrvX {
    int x;
    char c1;
};

class PrvY : public PrvX {
    char c2;
};

class PrvZ : public PrvY {
    char c3;
};

int main() {
    cout << "sizeof(PubX): " << sizeof(PubX) << endl;
    cout << "sizeof(PubY): " << sizeof(PubY) << endl;
    cout << "sizeof(PubZ): " << sizeof(PubZ) << endl;
    cout << "sizeof(PrvX): " << sizeof(PrvX) << endl;
    cout << "sizeof(PrvY): " << sizeof(PrvY) << endl;
    cout << "sizeof(PrvZ): " << sizeof(PrvZ) << endl;
}
