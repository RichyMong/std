#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <assert.h>

#ifdef DEBUG

static constexpr size_t head_size = 64;

void* operator new(size_t size, const char* file, int line)
{
    char* ptr = (char *) malloc(size + head_size);

    std::cout << "allocate " << size << " bytes: " << (void*)ptr << std::endl;

    snprintf(ptr, head_size, "%s:%d", file, line);

    return ptr + head_size;
}

void operator delete(void* ptr) noexcept
{
    std::cout << "free " << ptr << std::endl;

    char *pos = (char *)ptr;

    assert((size_t) pos > head_size);

    free(pos - head_size);
}

#endif
