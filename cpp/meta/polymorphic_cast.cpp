#include "polymorphic_cast.h"
#include <iostream>

struct A { virtual ~A() { } };

struct B : public A { ~B() { } };

int main() {
    using namespace std;

    B b;
    A* pa = &b;
    B* pb = ouou::polymorphic_downcast<B*>(pa);
    A& ra = b;
    B& rb = ouou::polymorphic_downcast<B&>(ra);
}
