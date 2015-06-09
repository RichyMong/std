#include <iostream>
#include <map>

using namespace std;

class X {
public:
    X(int i = 0) : val_ {i} { cout << "constructing " << i << endl; }
    ~X() { cout << "destructing " << val_ << endl; }

private:
    int val_;
};

int main()
{
    map<int, X> xmap;
    xmap[1] = X(1);
    xmap[2] = X(2);
}
