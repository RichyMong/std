#include <string>

class X {
public:
    X& get_x() { return *this; }
    void weight(int new_weight) { weight_ = new_weight; }
private:
    int weight_;
};

X get_x() {
    return X();
}

int main() {
    using namespace std;
    std::string str;

    get_x().weight(10);

    swap(string("abc").append("123"), str);
    // swap(str, (string("abc") + "123"));
    // str.swap(string("abc") + "123");
    str.swap(string("abc").append("123"));
    string("abc").append("123").swap(str);
    (string("abc") + "123").swap(str);
    return 0;
}
