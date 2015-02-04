#include <iostream>
#include <map>
#include <string>
#include <algorithm>

class Test {
public:
    void test(int type) const
    {
        static std::map<int, void (Test::*)() const> operations = {
            std::make_pair(0, &Test::foo),
            std::make_pair(1, &Test::bar),
        };
        auto it = operations.find(type);
        if (it != operations.cend())
            (this->*it->second)();
    }
private:
    void foo() const { std::cout << "foo" << std::endl; }
    void bar() const { std::cout << "bar" << std::endl; }
};

int main()
{
    Test x;
    x.test(0);
    x.test(1);
    std::string text { "abcdefghijklmn" };
    std::fill(text.begin(), text.begin() + std::min(3, (int)text.size()), '*');
    std::cout << text << std::endl;
}
