#include "util.h"
#include <cstdio>
#include <cstdarg>
#include <errno.h>
#include <string.h>

namespace util {

void __attribute__((noreturn)) err_exit(bool error, const char* fmt, ...) {
    fflush(stdout);

    va_list varg;

    va_start(varg, fmt);
    vfprintf(stderr, fmt, varg);
    va_end(varg);

    if (error) {
        fprintf(stderr, "%s: ", strerror(errno));
    }

    exit(EXIT_FAILURE);
}

void err_print(const char* fmt, ...) {
    fflush(stdout);

    va_list varg;

    va_start(varg, fmt);
    vfprintf(stderr, fmt, varg);
    va_end(varg);

    fprintf(stderr, "%s: ", strerror(errno));
}

}
