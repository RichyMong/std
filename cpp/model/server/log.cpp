#include "log.h"
#include <stdexcept>
#include <cstdio>
#include <cstdarg>
#include <unistd.h>

namespace util {

Log::Log(const std::string& path) {
    file_ = fopen(path.c_str(), "a");
    if (file_ == NULL) {
        throw std::runtime_error("cannot create log file");
    }
}

Log::Log(FILE* file)
    : file_ {file} {
}

Log::~Log() {
    fsync(fileno(file_));
    fclose(file_);
}

void Log::debug(const char* fmt, ...) {
    va_list varg;

    va_start(varg, fmt);
    fprintf(file_, fmt, varg);
    va_end(varg);
}

void Log::warning(const char* fmt, ...) {
    va_list varg;

    va_start(varg, fmt);
    fprintf(file_, fmt, varg);
    va_end(varg);
}

}
