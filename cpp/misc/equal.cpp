#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>

int main()
{
    std::vector<std::string> names { "jim", "james", "lilei" };
    auto cmpfun = std::bind(std::equal_to<std::string>(), std::placeholders::_1, "li");
    auto it = std::find_if(names.cbegin(), names.cend(), cmpfun);
    if (it != names.cend()) {
        std::cout << "yes, we find it." << std::endl;
    }

    return 0;
}
