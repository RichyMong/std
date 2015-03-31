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

    void use_x() {
        xsets.erase(shared_from_this());
        cout << "use_x done\n";
    }

private:
    int val_;
};

class Y {
public:
    explicit Y(const std::shared_ptr<X>& px)
        : px_ { px } {
    }

    void show() const {
        auto sp = px_.lock();
        if (sp) cout << "valid\n";
        else cout << "invalid\n";
    }

private:
    std::weak_ptr<X> px_;
};

int main() {
    xsets.insert(make_shared<X>(2));
    Y y(*xsets.begin());
    y.show();
    cout << xsets.size() << endl;
    (*xsets.begin())->use_x();
    cout << xsets.size() << endl;
    y.show();
}
