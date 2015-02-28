#include <iostream>
#include <string>
#include <algorithm>
#include <ctype.h>

int main() {
    std::string name = "arcrobot_0001";

    std::cout << std::all_of(name.cbegin(), name.cend(), isalnum) << std::endl;

    return 0;
}
