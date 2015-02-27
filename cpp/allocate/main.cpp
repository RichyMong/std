#include "allocator.h"
#include <iostream>
#include <memory>

namespace meng {

class Object {
    friend std::ostream& operator<<(std::ostream&, const Object&);
public:
    Object() = default;

    Object(int h, int w) : height_ {h}, weight_ {w} {
    }

    ~Object() {
        std::cout << *this << " is destructing\n";
    }

private:
    int height_;
    int weight_;
};

std::ostream& operator<<(std::ostream& os, const Object& obj) {
    os << "height " << obj.height_ << ", weight " << obj.weight_;
    return os;
}

}

int main()
{
    meng::Allocator<meng::Object> allocator { 10 };
    std::cout << "free slots: " << allocator.free_slots() << std::endl;
    auto p = allocator.alloc(1, 2);
    if (p) {
        std::cout << "free slots: " << allocator.free_slots() << std::endl;
        std::cout << *p << std::endl;
        allocator.dealloc(p);
    }
    std::cout << "free slots: " << allocator.free_slots() << std::endl;
}
