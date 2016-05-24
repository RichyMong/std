#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <assert.h>
#include <iostream>

using namespace std;

template <typename T, size_t N>
void EncodeCharArray(char *out, T (&array)[N])
{
    char* raw = (char *) array;
    auto offset = 0;

    int* p = (int*) array;

    auto dsize = sizeof(*p);
    auto ssize = sizeof(T);

    auto step = dsize <= ssize ? 1 : dsize / ssize;

    assert(ssize >= dsize || dsize % ssize == 0);

    for (int i = 0; i < N; i += step) {
        for (unsigned long j = 0; j < dsize; j += ssize) {
            int n = std::min(dsize - j, step * ssize);
            memcpy(out + offset, raw + offset, n);
            offset += n;
        }
    }
}

template <typename T, size_t N>
void DecodeCharArray(char *in, T (&array)[N])
{
    char* raw = (char *) array;
    auto offset = 0;

    int* p = (int*) array;

    auto dsize = sizeof(*p);
    auto ssize = sizeof(T);

    auto step = dsize <= ssize ? 1 : dsize / ssize;

    assert(ssize >= dsize || dsize % ssize == 0);

    for (int i = 0; i < N; i += step) {
        for (unsigned long j = 0; j < dsize; j += ssize) {
            int n = std::min(dsize - j, step * ssize);
            memcpy(raw + offset, in + offset, n);
            offset += n;
        }
    }
}

template <typename T, size_t N>
void print_array(T (&array)[N])
{
    for (int i = 0; i < N; ++i) {
        printf("0X%llx\n", array[i]);
    }
}

int main()
{
    char abc[] = "abcde";
    char out[sizeof(abc)];

    EncodeCharArray(out, abc);
    DecodeCharArray(out, abc);
    printf("%s\n", abc);

    short sarray[] = { 0x1234, 0x4321, 0x5678 };
    char sout[sizeof(short) * 3];
    EncodeCharArray(sout, sarray);
    DecodeCharArray(sout, sarray);
    print_array(sarray);

    long long larray[] = { 0x12345678abcdef0, 0x0fedcba87654321, 0x5678 };
    char lout[sizeof(long long) * 3];
    EncodeCharArray(lout, larray);
    DecodeCharArray(lout, larray);
    print_array(larray);
}
