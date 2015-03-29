#include <iostream>
#include <assert.h>

using namespace std;

class Point {
public:
    float x;
    float y;
};

template <class class_type,
          class data_type1,
          class data_type2>
const char* access_order(
    data_type1 class_type::*member1,
    data_type2 class_type::*member2) {
    assert(member1 != member2);
    return (static_cast<void*>(member1) < static_cast<void*>(member2))
                    ? "member1 occurs first"
                    : "member2 occurs first";
}

int main() {
    Point pt;
    cout << "member x address: " << &Point::x << endl;
    cout << "member y address: " << &Point::y << endl;
    cout << "Point address: " << &pt << endl;
    cout << "member x address: " << &pt.x << endl;
    cout << "member y address: " << &pt.y << endl;
    cout << access_order(&Point::x, &Point::y) << endl;
}
