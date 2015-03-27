#include "log.h"
#include "time.h"
#include <stdexcept>
#include <cstdio>
#include <cstdarg>
#include <unistd.h>
#include <string.h>
#include <assert.h>

namespace util {

#define do_output(_level, _level_str)                \
    do {                                             \
        if (level_ < (_level)) {                     \
            return;                                  \
        }                                            \
        va_list varg;                                \
        va_start(varg, fmt);                         \
        output(_level_str, fmt, varg);               \
        va_end(varg);                                \
    } while (0)                                      \


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

void Log::output(const char* level, const char* fmt, va_list varg) {
    assert(fmt && fmt[0]);

    char buf[BUFSIZ];

    auto nwrite = snprintf(buf, sizeof(buf), "%s <%s> ", cached_log_time, level);

    nwrite += vsnprintf(buf + nwrite, sizeof(buf) - nwrite, fmt, varg);
    nwrite += snprintf(buf + nwrite, sizeof(buf) - nwrite, "\n");
    if (nwrite >= (int) sizeof(buf)) {
        auto end = buf + sizeof(buf) - 3;
        auto pos = strrchr(buf, ' ');
        if (pos == NULL) {
            pos = end - 5;
        }
        while (pos != end) {
            *pos++ = '.';
        }
        nwrite = sizeof(buf);
    }

    // fwrite is MT-Safe so we do not use any lock here.
    fwrite(buf, nwrite, 1, file_);
}

void Log::debug(const char* fmt, ...) {
    do_output(LogLevel::kDebug, "D");
}

void Log::warning(const char* fmt, ...) {
    do_output(LogLevel::kWarn, "W");
}

void Log::info(const char* fmt, ...) {
    do_output(LogLevel::kInfo, "I");
}

void Log::error(const char* fmt, ...) {
    do_output(LogLevel::kError, "E");
}

}
