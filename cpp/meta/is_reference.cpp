#include "is_reference.h"
#include "is_array.h"
#include <iostream>

class X { };

int main() {
    using namespace std;
    cout << ouou::is_reference<int>::value << endl;
    cout << ouou::is_reference<int&>::value << endl;
    cout << ouou::is_reference<X>::value << endl;
    cout << ouou::is_reference<X&>::value << endl;
    cout << ouou::is_array<char[3]>::value << endl;
    cout << ouou::is_array<int[5]>::value << endl;
}
