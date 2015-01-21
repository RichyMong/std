#include <iostream>
#include <iomanip>
#include <sstream>

int main()
{
    std::ostringstream oss;
    oss << std::setprecision(1) << std::fixed << 123.567 << " " << "abc" << std::endl;
    std::cout << oss.str();

    return 0;
}
