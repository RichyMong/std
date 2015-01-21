#include <iostream>

template <typename T>
class Smartptr {
public:
    Smartptr() : _raw { nullptr } {}
    Smartptr(T *p) : _raw {p} {}
    T* operator ->() { return _raw; }
    operator T*() { return _raw; }
private:
    T *_raw;
};

int main()
{
    Smartptr<int> p;
    if (p) {
        std::cout << "unexpected" << std::endl;
    }
}
