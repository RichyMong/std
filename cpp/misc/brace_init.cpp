#include <string>

using std::string;

class Test {
public:
    Test(int &x, const string& s) : x_ { x }, s_ {s} { }

private:
    int x_;
    string s_;
};

int main()
{
    int x = 1;
    string s { "abcde" };
    Test test(x, s);
}
