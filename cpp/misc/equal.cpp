#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <string.h>

class X {
public:
    char name[12];
    int  x;
    short y;

    X() { memset(this, 0, sizeof(*this)); }
};

int main()
{
    std::vector<std::string> names { "jim", "james", "lilei" };
    auto cmpfun = std::bind(std::equal_to<std::string>(), std::placeholders::_1, "li");
    auto it = std::find_if(names.cbegin(), names.cend(), cmpfun);
    if (it != names.cend()) {
        std::cout << "yes, we find it." << std::endl;
    }

    X x1, x2;
    sprintf(x1.name, "Jim");
    x1.x = 2;
    x1.y = 3;
    sprintf(x2.name, "Jimmy");
    x2.x = 2;
    x2.y = 3;
    std::cout << (memcmp(&x1, &x2, sizeof(x1))) << std::endl;

    return 0;
}
