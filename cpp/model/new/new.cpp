#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <assert.h>

#ifdef DEBUG

static constexpr uint64_t magic = 0x01332bfd;
static constexpr size_t head_size = 64; // > sizeof(magic)

void* operator new(size_t size, const char* file, int line)
{
    char* ptr = (char *) malloc(size + head_size);

    std::cout << "allocate " << size << " bytes: " << (void*)ptr << std::endl;

    *(uint64_t*) ptr = magic;

    snprintf(ptr + sizeof(magic), head_size - sizeof(magic), "%s:%d", file, line);

    return ptr + head_size;
}

void operator delete(void* ptr) noexcept
{
    std::cout << "try to free " << ptr << std::endl;

    if (!ptr) {
        return;
    }

    char* cptr = (char *) ptr - head_size;
    if (*(decltype(magic)*) (cptr) != magic) {
        free(ptr);
    } else {
        free(cptr);
    }

}

#endif
