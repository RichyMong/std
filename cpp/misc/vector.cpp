#include <iostream>
#include <vector>
#include <algorithm>
#include <type_traits>

using namespace std;

struct X {
    X()
    {
        cout << "calling x constructor\n";
    }

    X(const X& x)
    {
        cout << "calling x copy constructor\n";
    }
    X& operator=(const X& x)
    {
        cout << "calling x assign constructor\n";
        return *this;
    }
};

struct A {
    vector<X> vx;
    X x;
};

std::vector<int> f(size_t size)
{
    std::vector<int> v;
    v.reserve(size);
    for (size_t i = 0; i != size; ++i)
        v.push_back(10);

    return v;
}

void reserve_test()
{
    char buf[] = "abcdefg";
    std::vector<char> v;
    v.reserve(128);
    v.push_back('0');
    v.push_back('1');
    std::copy(buf, buf + sizeof(buf) - 1, std::back_inserter(v));
    std::cout << v.size() << ", capacity " << v.capacity() << std::endl;
}

template <typename T>
void is_vector(const T& t)
{
    cout << "is not vector\n";
}

template <typename T>
void is_vector(const vector<T>& t)
{
    cout << "is vector\n";
}

int main()
{
    auto x = f(20);
    std::cout << x.size() << std::endl;
    reserve_test();

    vector<int> vi;
    vi.reserve(6);
    cout << "vi size: " << vi.size() << endl;
    vi.insert(vi.end(), {10, 20, 30});
    cout << "vi size: " << vi.size() << endl;
    vi.insert(vi.end(), {40, 50, 60});
    cout << "vi size: " << vi.size() << endl;
    for_each(vi.begin(), vi.end(), [](int v) { cout << v << endl; });

    X xx;
    A a;
    cout << "assigning vector\n";
    a.vx = vector<X>(&xx, &xx + 1);
    cout << "assigned vector\n";
    a.x = X();
    cout << "swap\n";
    vector<X> tmpvx(&xx, &xx + 1);
    a.vx.swap(tmpvx);
    cout << "testing vector is_vector\n";
    is_vector(vi);
    is_vector(a.vx);
    cout << "testing int is_vector\n";
    is_vector(6);
}
