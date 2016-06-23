#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

class X {
public:
    X(int i = 0) : val_ {i} { cout << "constructing " << i << endl; }
    ~X() { cout << "destructing " << val_ << endl; }

    bool operator<(const X& rhs) const { return val_ < rhs.val_; }

private:
    int val_;
};

int main()
{
    map<int, X> xmap;
    xmap[1] = X(1);
    xmap[2] = X(2);
    xmap.insert(make_pair(1, X(3)));

    cout << "map init end\n";

    set<X> xset;
    xset.erase(X(4));
    xset.insert(X(4));

    map<int, set<X>> xvec;
    xvec[1].insert(X(1));
    xvec[2].erase(X(1));
    if (xvec[2].empty()) {
        cout << "it's empty\n";
    }

    cout << xset.insert(X(4)).second << endl;
    cout << xset.insert(X(5)).second << endl;
}
