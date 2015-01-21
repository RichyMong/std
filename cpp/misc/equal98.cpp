#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>

int main()
{
    std::vector<std::string> names;
    names.push_back("lilei");
    names.push_back("jim");
    std::vector<std::string>::const_iterator it = std::find_if(names.begin(), names.end(), std::bind2nd(std::equal_to<std::string>(), "jim"));
    if (it != names.end()) {
        std::cout << "yes, we find it." << std::endl;
    }

    return 0;
}
