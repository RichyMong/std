#include <iostream>

template <typename T>
void update(const T* data)
{
    std::cout << "Address: " << reinterpret_cast<const void*>(data) <<
                 ", value: " << data << std::endl;
}

void update(const int* data)
{
    std::cout << "Update: " << data << std::endl;
}

int main()
{
    int __attribute__((aligned(1))) b = 0x43;
    short c = 0x4241;
    char a = 'a';
    update(&a);
    update(&b);
    update(&c);

    return 0;
}
