#include <iostream>
#include <assert.h>

using namespace std;

class Point2d {
public:
    virtual void sum() { }

    float x;
    float y;
};

class Point3d : public Point2d {
public:
    float z;
};

template <class class_type,
          class data_type1,
          class data_type2>
const char* access_order(
    data_type1 class_type::*member1,
    data_type2 class_type::*member2) {
    assert(member1 != member2);
    return ((member1) < (member2))
                    ? "member1 occurs first"
                    : "member2 occurs first";
}

int main() {
    float Point2d::*nullp = 0;
    cout << "Point2d::null address: " << nullp << endl;
    Point2d pt;
    cout << "Point2d::x address: " << &Point2d::x << endl;
    cout << "Point2d::y address: " << &Point2d::y << endl;
    cout << "Point3d::x address: " << &Point3d::x << endl;
    cout << "Point3d::y address: " << &Point3d::y << endl;
    cout << "Point3d::z address: " << &Point3d::z << endl;
    cout << "Point2d address: " << &pt << endl;
    cout << "member x address: " << &pt.x << endl;
    cout << "member y address: " << &pt.y << endl;
}
