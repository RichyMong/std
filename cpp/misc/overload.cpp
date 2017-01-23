#include <iostream>

using namespace std;

template <typename T>
void test(const T* t)
{
    cout << *t << " pointer\n";
}

template <typename T>
void test(const T& t)
{
    cout << sizeof(t) << " reference\n";
    test(&t);
}

int main()
{
    int i = 0;
    test(&i);
    test(i);
}
