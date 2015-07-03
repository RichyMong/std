#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;

template <typename UnaryFunction>
void for_each_splited_string(const std::string& str,
                     const char* delimeter,
                     UnaryFunction func)
{
    std::string::size_type start, end;

    for (start = 0;
         (end = str.find_first_of(delimeter, start)) != std::string::npos;
         start = str.find_first_not_of(delimeter, end)) {
        func(str.substr(start, end - start));
    }
}

std::vector<string> split_string(const std::string& str, const char* delimeter)
{
    std::vector<std::string> splited;

    for_each_splited_string(str, delimeter,
            [&splited](const std::string& str) { splited.push_back(str); });

    return splited;
}

int main()
{
    auto print = [](const std::string& str){ cout << str << endl; };

    for_each_splited_string("ab,:cd-efg:", ",:-", print);

    auto slices = split_string("ab,:cd-efg:", ",:-");

    std::for_each(slices.cbegin(), slices.cend(), print);

    std::ostream_iterator<std::string> oit(std::cout);

    std::copy(slices.cbegin(), slices.cend(), oit);
}
