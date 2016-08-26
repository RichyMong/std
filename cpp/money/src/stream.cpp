#include <iostream>
#include <string>
#include "stream.h"

int main(int argc, const char *argv[])
{
    (void) argc;
    (void) argv;

    util::OutputBufferStream<kSmallBuf> bs;
    bs.write((short) 2);
    bs.write(std::string("hello"));
    bs.write(std::vector<int>{1, 2, 3, 4, 5, 6});
    int ai[] = {10, 100, 100};
    bs.write(ai);
    std::string abc { "we are here" };
    bs.write<uint>(abc);

    util::InputBufferStream rbs(bs.buffer_, bs.offset_);
    short x = rbs.readShort();
    std::cout << x << std::endl;

    std::string s;
    if (rbs.read(s)) {
        std::cout << s << std::endl;
    }

    std::vector<int> vec;
    if (rbs.read(vec)) {
        std::cout << vec.size() << std::endl;
        std::cout << vec[0] << std::endl;
        std::cout << vec[5] << std::endl;
    }

    int aii[3];
    rbs.read(aii);
    std::cout << aii[0] << ", " << aii[2] << std::endl;
    std::cout << rbs.readString<uint>() << std::endl;

    return 0;
}
