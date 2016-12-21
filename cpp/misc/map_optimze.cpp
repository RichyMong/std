#include <map>
#include <iostream>

using namespace std;

struct X {
    X() { cout << "constructing x\n"; }
    X(const X&) { cout << "copy constructing x\n"; }
    X& operator=(const X&) {
        cout << "assigning x\n";
        return *this;
    }
};

X GetX()
{
    X x;
    cout << "getting x\n";
    return x;
}

int main()
{
    std::map<int, int> imap { { 1, 1234 }, { 2, 4567 }, { 3, 8912 } };
    auto it = imap.find(3);
    if (it != imap.end()) {
        cout << imap[3] << endl;
    }

    auto y = GetX();
    map<int, X> mx;
    cout << "inserting 1\n";
    mx[1] = GetX();
    cout << "inserting 2\n";
    mx[2] = GetX();

    return 0;
}


