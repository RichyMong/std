#include <iostream>

template <typename T>
class Singleston {
public:
    static T& instance() {
        static T t;
        return t;
    }
    Singleston() { }

private:
    Singleston(const Singleston&);
    Singleston& operator =(const Singleston&);
};

class A : public Singleston<A> {
    friend class Singleston<A>;
public:
    void show() const { std::cout << val_ << std::endl; }
private:
    A(int i = 3) : val_ { i } { }
    int val_;
};

int main()
{
    A::instance().show();
}
