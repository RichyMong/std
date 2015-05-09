#include <iostream>
#include <cstdlib>

class Test {
public:
    void* operator new(size_t size) {
        std::cout << "operator new, size: " << size << std::endl;
        return ::malloc(size);
    }
    void operator delete(void* data, size_t size) {
        std::cout << "delete object, size: " << size << std::endl;
        ::free(data);
    }
private:
    int val_;
};

int main() {
    auto x = new Test();
    delete x;
}
