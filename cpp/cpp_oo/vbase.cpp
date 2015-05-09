#include <iostream>

using namespace std;

class ABase{
    int iMem[3];
};

class BBase : public virtual ABase {
    int iMem;
    int iMem2;
};

class CBase : public virtual ABase {
    int iMem;
    int iMem2;
    int iMem3;
};

class ABCDerived : public BBase, public CBase {
        int iMem;
};

int main() {
    cout << sizeof(ABase) << endl;
    cout << sizeof(BBase) << endl;
    cout << sizeof(CBase) << endl;
    cout << sizeof(ABCDerived) << endl;
}
