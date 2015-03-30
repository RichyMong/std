#include <iostream>
#include <memory>
#include <set>

using namespace std;

class X;

std::set<std::shared_ptr<X>> xsets;

class X : public enable_shared_from_this<X> {
public:
    X(int val) : val_ { val } { }

    ~X() { cout << "destructing " << val_ << endl; }

    int use_x() {
        xsets.erase(shared_from_this());
        cout << "use_x done\n";
        return val_;
    }

private:
    int val_;
};

int main() {
    xsets.insert(make_shared<X>(2));
    cout << xsets.size() << endl;
    (*xsets.begin())->use_x();
    cout << xsets.size() << endl;
}
