#ifndef MENG_ALLOCATOR_H
#define MENG_ALLOCATOR_H

#include <stack>
#include <iostream>

namespace meng {

template <typename T>
class Allocator {
public:
    Allocator(std::size_t size)
        : size_ {size}, block_ { new char[sizeof(T) * size] } {
        for (size_t i = 0; i != size; ++i) {
            unused_.push(block_ + i * sizeof(T));
        }
    }

    template <typename... Args>
    T* alloc(Args... args) {
        if (unused_.empty()) {
            std::cerr << "out of memory\n";
            return nullptr;
        }

        static_assert(std::is_constructible<T, Args...>(), "cannot construct!");

        auto fmem = unused_.top();
        unused_.pop();
        return new(fmem)T(args...);
    }

    void dealloc(T* p) {
        p->~T();
        unused_.push(reinterpret_cast<char*>(p));
    }

    std::size_t free_slots() const {
        return unused_.size();
    }

private:
    char*             block_;
    std::size_t       size_;
    std::stack<char*> unused_;
};

}

#endif // MENG_ALLOCATOR_H

