#include <iostream>

#define SIZE 10
#define STR(n) #n
#define ISTR(i) STR(i)
#define CONT(x, y) (x ":" STR(y) ":" STR(5))

void print(const char *text = CONT(__FILE__, __LINE__))
{
    std::cout << text << std::endl;
    std::cout << STR(SIZE) << std::endl;
    std::cout << ISTR(SIZE) << std::endl;
}

int main()
{
    print();
    print();

    return 0;
}
