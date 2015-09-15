#ifndef LEET_CODE_SOLUTION_H
#define LEET_CODE_SOLUTION_H

#include <iostream>

template <class Iterator>
void pretty_output(Iterator start, Iterator end)
{
    std::cout << '[';
    while (start != end) {
        std::cout << *start;
        if (++start != end)
            std::cout << ", ";
    }
    std::cout << ']' << std::endl;
}

#endif
