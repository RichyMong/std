#include <unistd.h>
#include <util/const.h>
#include <iostream>
#include "header.h"

int main(int argc, char** argv)
{
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);
    std::cout << sizeof(RawHeader) << std::endl;
}

