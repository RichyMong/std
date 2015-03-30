#include <iostream>
#include <memory>

using namespace std;

class X : public enable_shared_from_this<X> {
public:
    shared_ptr<X> getptr() {
        return shared_from_this();
    }
};

int main() {
    auto sp = make_shared<X>();
    shared_ptr<X> y = sp.get()->getptr();
    cout << sp.use_count() << endl;
}
