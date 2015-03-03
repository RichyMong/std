#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <errno.h>
#include <string.h>

namespace util {

void __attribute__((noreturn)) err_exit(bool error, const char* fmt, ...) {
    va_list varg;

    fflush(stdout);

    if (error) {
        fprintf(stderr, "%s: ", strerror(errno));
    }

    va_start(varg, fmt);
    vfprintf(stderr, fmt, varg);
    va_end(varg);

    exit(EXIT_FAILURE);
}

}
