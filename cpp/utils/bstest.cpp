#include  <cstdio>
#include <iostream>
#include "bitstream.h"

template <typename T, size_t N>
constexpr size_t array_size(const T (&array)[N])
{
    return N;
}

void test_case_1()
{
    printf("---%s started---\n", __FUNCTION__);

    util::BitStream<512, false> bs;

    bs.write(0x12, 5);
    bs.write(0xff, 8);

    size_t uelem[] = { 65535, 2147483648, 78 };
    bs.write(uelem);

    bs.write(0x77, 8);
    bs.write(0x1234, 16);

    char result[] = "abcdefg";
    bs.write(result);

    bs.write(0xffff, 7);

    short selem[] = { -20, 32767, -32768 };
    bs.write(selem);

    bs.seek(0);

    printf("%d\n", bs.nbits());

    printf("0X%x\n", bs.read(5));
    printf("0X%x\n", bs.read(8));

    size_t uout[array_size(uelem)];
    bs.read(uout);
    printf("%zu %zu %zu\n", uout[0], uout[1], uout[2]);

    printf("0X%x\n", bs.read(8));
    printf("0X%x\n", bs.read(16));

    char out[sizeof(result)];
    bs.read(out);
    printf("%s\n", out);

    printf("0X%x\n", bs.read(7));

    short sout[array_size(selem)];
    bs.read(sout);
    printf("%d %d %d\n", sout[0], sout[1], sout[2]);
}

void test_case_2()
{
    printf("---%s started---\n", __FUNCTION__);

    util::BitStream<64, false> bs;

    bs.write(0x12, 5);
    bs.write(0xff, 8);

    bs.seek(0);
    bs.write(0x09, 5);
    bs.seek(0);

    printf("0x%x\n", bs.read(5));
    printf("0x%x\n", bs.read(8));
}

void test_case_3()
{
    printf("---%s started---\n", __FUNCTION__);

    util::BitStream<512, false> bs;

    bs.write(0x12, 5);
    bs.write(0xff, 8);
    bs.write(0x77, 8);
    bs.write(0x1234, 16);

    char result[] = "abcdefg";
    bs.write(result);

    bs.write(0xffff, 7);

    short selem[] = { -20, 32767, -32768 };
    bs.write(selem);

    bs.seek(0);

    printf("number of bits: %d\n", bs.nbits());

    printf("0X%x\n", bs.read(5));
    printf("0X%x\n", bs.read(8));
    printf("0X%x\n", bs.read(8));
    printf("0X%x\n", bs.read(16));

    char out[sizeof(result)];
    bs.read(out);
    printf("%s\n", out);

    printf("0X%x\n", bs.read(7));

    short sout[array_size(selem)];
    bs.read(sout);
    printf("%d %d %d\n", sout[0], sout[1], sout[2]);
}

int main()
{
    test_case_1();
    test_case_2();
    test_case_3();

    return 0;
}
