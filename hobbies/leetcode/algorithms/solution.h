#ifndef LEET_CODE_SOLUTION_H
#define LEET_CODE_SOLUTION_H

#include <iostream>

template <class Iterator>
inline void pretty_print(Iterator start, Iterator end)
{
    std::cout << '[';
    while (start != end) {
        std::cout << *start;
        if (++start != end)
            std::cout << ", ";
    }
    std::cout << ']' << std::endl;
}

template <class Container>
inline void pretty_print(const Container& c)
{
    pretty_print(std::begin(c), std::end(c));
}

#endif
