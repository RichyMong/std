#pragma once

#include <cstddef>

#ifdef DEBUG

void* operator new(size_t size, const char* file, int line);
void operator delete(void* ptr) noexcept;

#define new new(__FILE__, __LINE__)

#endif
