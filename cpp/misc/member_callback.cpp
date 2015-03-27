#include <iostream>
#include <functional>

class Call {
public:
    explicit Call(int val)
        : val_ {val} {
    }

    void work() { std::cout << val_ << std::endl; }

private:
    int val_;
};

void callback(std::function<void()> func) {
    func();
}

int main() {
    Call call {2};
    callback(std::bind(&Call::work, &call));
    return 0;
}
