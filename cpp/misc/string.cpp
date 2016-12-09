#include <iostream>
#include <string>
#include <vector>

using namespace std;

    static union {
        int x;
        long y;
    };
struct Test {

    Test() { y = 100; }
};

int main() {
    std::string s = "abc";
    std::cout << "size: " << s.size() << std::endl;
    Test x;
    std::cout << y << '\n';


    cout << "sizeof(string)=" << sizeof(string) << '\n';
    cout << "sizeof(string(\"hi\"))=" << sizeof(string("hi")) << '\n';
    std::string a[10];
    std::cout << "sizeof(string[10])=" << sizeof(a) << '\n';
    std::vector<std::string> v(10);
    cout << "sizeof(vector)=" << sizeof(vector<string>) << '\n';
    std::cout << "sizeof(vector<string>(10))="  <<sizeof(v) + sizeof(std::string) * v.capacity() << "\n";

}
