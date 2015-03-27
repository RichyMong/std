#include <iostream>

using namespace std;

class X { };
class Y : public virtual X { };
class Z : public virtual X { };
class A : public Y, public Z { };

int main() {
    std::cout << "sizeof(X): " << sizeof(X) << std::endl;
    std::cout << "sizeof(Y): " << sizeof(Y) << std::endl;
    std::cout << "sizeof(Z): " << sizeof(Z) << std::endl;
    std::cout << "sizeof(A): " << sizeof(A) << std::endl;
    return 0;
}
