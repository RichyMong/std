#include <boost/circular_buffer.hpp>
#include <string>
#include <iostream>

using namespace std;

int main()
{
    boost::circular_buffer<char> cbc(5);
    cbc.push_back('a');
    cbc.push_back('b');
    cbc.push_back('c');
    cout << cbc.size() << endl;
    cbc.push_back('d');
    cbc.push_back('e');
    std::cout << std::string(cbc.begin(), cbc.end()) << std::endl;
    cbc.erase_begin(5);
    cbc.push_back('f');
    std::cout << std::string(cbc.begin(), cbc.end()) << std::endl;

    return 0;
}
