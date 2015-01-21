#include <iostream>

class Base {
public:
    virtual void do_work(int x) const {
        std::cout << "do_work stub" << std::endl;
    }

    virtual void dispatch(int x) const {
        switch (x) {
        case 1:
            std::cout << "one" << std::endl;
            break;
        default:
            std::cout << "default in Base" << std::endl;
            do_work(x);
            break;
        }
    }
};

class Derived : public Base {
public:
    void dispatch(int x) const override {
        switch (x) {
        case 0:
            std::cout << "zero" << std::endl;
            break;
        default:
            Base::dispatch(x);
            break;
        }
    }
    void do_work(int x) const override {
        std::cout << "do real work" << std::endl;
    }

};

int main()
{
    Base *b = new Derived;
    b->dispatch(2);
    b->do_work(2);
}
