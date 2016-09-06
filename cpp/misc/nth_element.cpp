#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

struct X {
    int a;
    int b;
    int c;
};

template <typename T, typename M, M T::*p>
struct Cmp {
    bool operator()(const T& t1, const T& t2) const
    {
        return t1.*p < t2.*p;
    }
};

int main()
{
    vector<X> vx;

    for (int i = 0; i < 11; ++i) {
        vx.push_back(X { i + 1, 11 - i, i * i });
    }

    nth_element(vx.begin(), vx.begin(), vx.end(), Cmp<X, int, &X::b>());
    auto it = vx.begin();
    cout << it->a  << " " << it->b << endl;
}
